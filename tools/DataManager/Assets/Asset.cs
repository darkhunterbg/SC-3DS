using CsvHelper;
using CsvHelper.Configuration.Attributes;
using System;
using System.Collections.Generic;
using System.IO;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Reflection;
using CsvHelper.Configuration;
using CsvHelper.TypeConversion;
using System.Numerics;
using DataManager.Assets.Raw;

namespace DataManager.Assets
{

	public abstract class Asset
	{
		[Ignore]
		public abstract string AssetName { get; }

		[Ignore]
		public virtual string SortKey => AssetName;

		[Ignore]
		public virtual GuiTexture Preview => null;

		[Ignore]
		public virtual string ActionButtonText => null;

		[Ignore]
		[UniqueKey]
		public readonly int Id;

		static int id = 0;

		public Asset() { Id = ++id; }

		public override string ToString()
		{
			return AssetName;
		}


		public virtual void Activate() { }
		public virtual Asset Clone()
		{
			var asset = (Asset)Util.ShallowCopyProperties(GetType(), this);
			return asset;
		}

		public static TAsset New<TAsset>(TAsset copy) where TAsset : Asset
		{
			if (copy == null)
			{
				var r = Activator.CreateInstance<TAsset>();
				r.OnAfterDeserialize();
				return r;
			}
			var clone = copy.Clone() as TAsset;

			return clone;
		}

		public virtual void OnBeforeSerizalize() { }
		public virtual void OnAfterDeserialize() { }

		public class AssetConverter : DefaultTypeConverter
		{
			public override object ConvertFromString(string text, IReaderRow row, MemberMapData memberMapData)
			{
				var type = memberMapData.Type;
				var asset = AppGame.AssetManager.Assets[type]
						.Assets.FirstOrDefault(a => a.AssetName == text);

				return asset;
			}
			public override string ConvertToString(object value, IWriterRow row, MemberMapData memberMapData)
			{
				if (value == null)
					return string.Empty;
				return ((Asset)value).AssetName;
			}
		}

	}

	public  struct AssetId
	{
		static uint @base = (uint)(DateTime.Now - new DateTime(2021,1,1)).TotalSeconds;

		public readonly uint Id;

		public override string ToString()
		{
			return Id.ToString();	
		}

		public AssetId(uint id) { Id = id; }

		internal static void Reseed(uint seed)
		{
			@base = seed;
		}

		public static AssetId New()
		{
			return new AssetId(++@base);
		}

		public class CsvConverter : DefaultTypeConverter
		{
			public override object ConvertFromString(string text, IReaderRow row, MemberMapData memberMapData)
			{
				if (!uint.TryParse(text, out uint id))
					return AssetId.New();

				if (id > AssetId.@base)
					AssetId.Reseed(id);

				return new AssetId(id);
			}

			public override string ConvertToString(object value, IWriterRow row, MemberMapData memberMapData)
			{
				return ((AssetId)value).Id.ToString();
			}
		}
	}

	public interface IAssetDatabase
	{
		Type Type { get; }

		public IEnumerable<Asset> Assets { get; }

		void Reload();
		void Save();
		void PrepareForSerialization();
	}

	public class AssetDatabase<TAsset> : IAssetDatabase where TAsset
		: Asset, new()
	{
		private readonly Type type = typeof(TAsset);
		public string FilePath { get; private set; }

		Type IAssetDatabase.Type => type;


		private List<TAsset> _assets = new List<TAsset>();
		IEnumerable<Asset> IAssetDatabase.Assets => Assets;

		public IReadOnlyList<TAsset> Assets => _assets;

		public AssetDatabase(string file)
		{
			FilePath = file;
		}

		static void AddConverters(CsvContext context)
		{
			context.TypeConverterCache.AddConverter<Asset.AssetConverter>(new Asset.AssetConverter());

			context.TypeConverterCache.AddConverter<IconRef>(new IconRef.IconConverter());

			context.TypeConverterCache.AddConverter<ImageListRef>(new ImageListRef.CsvConverter());

			context.TypeConverterCache.AddConverter<ImageFrameRef>(new ImageFrameRef.CsvConverter());

			context.TypeConverterCache.AddConverter<Vector2>(new CsvConverters.Vector2Coverter());

			context.TypeConverterCache.AddConverter<AssetId>(new AssetId.CsvConverter());

			context.TypeConverterCache.AddConverter<AudioClipRef>(new AudioClipRef.CsvConverter());
		}

		public void Reload()
		{
			_assets.Clear();
			if (!File.Exists(FilePath))
				return;
			using (var csv = new CsvReader(new StreamReader(FilePath), AssetManager.CsvConfig))
			{
				AddConverters(csv.Context);

				_assets.AddRange(csv.GetRecords<TAsset>());
			}

			foreach (var s in Assets)
			{
				s.OnAfterDeserialize();
			}
		}

		public void Save()
		{
			PrepareForSerialization();

			foreach (var s in Assets)
			{
				s.OnBeforeSerizalize();
			}

			using (var csv = new CsvWriter(new StreamWriter(FilePath), AssetManager.CsvConfig))
			{
				AddConverters(csv.Context);

				csv.WriteRecords(Assets);
			}
		}

		public void Add(TAsset asset)
		{
			_assets.Add(asset);
		}
		public void AddRange(IEnumerable<TAsset> asset)
		{
			_assets.AddRange(asset);
		}
		public TAsset New(TAsset copy)
		{
			var asset = Asset.New(copy);
			_assets.Add(asset);
			return asset;
		}
		public void Delete(TAsset asset)
		{
			_assets.Remove(asset);

			foreach (var db in AppGame.AssetManager.Assets)
			{
				if (db.Key == typeof(TAsset))
					continue;

				var refProp = db.Key.GetProperties().Where(t => t.PropertyType == typeof(TAsset))
					.ToList();

				if (refProp.Count == 0)
					continue;

				foreach (var a in db.Value.Assets)
				{
					foreach (var p in refProp)
					{
						var v = p.GetValue(a);
						if (v == asset)
						{
							p.SetValue(a, null);
						}
					}
				}
			}
		}
		public void DeleteAll(Predicate<TAsset> predicate)
		{
			var removed = _assets.Where(t => predicate(t)).ToList();

			_assets.RemoveAll(t=>removed.Contains(t));

			foreach (var db in AppGame.AssetManager.Assets)
			{
				if (db.Key == typeof(TAsset))
					continue;

				var refProp = db.Key.GetProperties().Where(t => t.PropertyType == typeof(TAsset))
					.ToList();

				if (refProp.Count == 0)
					continue;

				foreach (var a in db.Value.Assets)
				{
					foreach (var p in refProp)
					{
						var v = p.GetValue(a);
						if (removed.Contains(v))
						{
							p.SetValue(a, null);
						}
					}
				}
			}
		}

		public void PrepareForSerialization()
		{
			var a = Assets.OrderBy(s => s.SortKey).ToList();
			_assets.Clear();
			_assets.AddRange(a);
		}
	}
}
