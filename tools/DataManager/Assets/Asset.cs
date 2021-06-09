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

namespace DataManager.Assets
{

	public abstract class Asset 
	{
		[Ignore]
		public abstract string AssetName { get; }

		[Ignore]
		public virtual GuiTexture Preview => null;

		[Ignore]
		[UniqueKey]
		public readonly int Id;

		static int id = 0;

		public Asset() { Id = ++id; }

		public override string ToString()
		{
			return AssetName;
		}

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

	public interface IAssetDatabase
	{
		Type Type { get; }

		public IEnumerable<Asset> Assets { get; }

		void Reload();
		void Save();
	}

	public class AssetDatabase<TAsset> : IAssetDatabase where TAsset
		: Asset , new()
	{
		private readonly Type type = typeof(TAsset);
		public string FilePath { get; private set; }

		Type IAssetDatabase.Type => type;

		IEnumerable<Asset> IAssetDatabase.Assets => Assets;

		public List<TAsset> Assets { get; private set; } = new List<TAsset>();

		public AssetDatabase(string file)
		{
			FilePath = file;
		}

		static void AddConverters(CsvContext context)
		{
			context.TypeConverterCache.AddConverter<Asset.AssetConverter>(new Asset.AssetConverter());

			context.TypeConverterCache.AddConverter<IconRef.IconConverter>(new IconRef.IconConverter());

			context.TypeConverterCache.AddConverter<ImageListRef.CsvConverter>(new ImageListRef.CsvConverter());

			context.TypeConverterCache.AddConverter<ImageFrameRef.CsvConverter>(new ImageFrameRef.CsvConverter());
		}

		public void Reload()
		{
			Assets.Clear();
			if (!File.Exists(FilePath))
				return;
			using (var csv = new CsvReader(new StreamReader(FilePath), AssetManager.CsvConfig))
			{
				AddConverters(csv.Context);

				Assets.AddRange(csv.GetRecords<TAsset>());
			}

			foreach (var s in Assets)
			{
				s.OnAfterDeserialize();
			}
		}

		public void Save()
		{
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


		public TAsset New(TAsset copy)
		{
			var asset = Asset.New(copy);
			Assets.Add(asset);
			return asset;
		}
		public void Delete(TAsset asset)
		{
			Assets.Remove(asset);
		}
	}
}
