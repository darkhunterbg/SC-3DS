using CsvHelper;
using CsvHelper.Configuration.Attributes;
using System;
using System.Collections.Generic;
using System.IO;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Reflection;

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

		public Asset() { id = ++id; }

		public override string ToString()
		{
			return AssetName;
		}

		public virtual Asset Clone()
		{
			return Util.ShallowCopyProperties(this);
		}

		public static TAsset New<TAsset>(TAsset copy) where TAsset : Asset
		{
			if (copy == null)
			{
				var r = Activator.CreateInstance<TAsset>();
				r.OnAfterDeserialize();
				return r;
			}
			return copy.Clone() as TAsset;
		}


		public virtual void OnAfterDeserialize()
		{
			var assetRefFields = GetType().GetFields(BindingFlags.Instance |
				BindingFlags.SetField | BindingFlags.Public| BindingFlags.NonPublic)
				.Where(p => p.GetCustomAttribute<AssetReferenceAttribute>() != null).ToList();

			foreach (var r in assetRefFields)
			{
				var attr = r.GetCustomAttribute<AssetReferenceAttribute>();
				var type = r.FieldType;
				var key = GetType().GetProperty(attr.KeyPropertyName).GetValue(this) as string;

				if (!string.IsNullOrEmpty(key))
				{
					var asset = AppGame.AssetManager.Assets[type]
						.Assets.FirstOrDefault(a => a.AssetName == key);
					if (asset != null)
					{
						r.SetValue(this, asset);
					}
				}
			}

		}
	}

	[AttributeUsage(AttributeTargets.Field, AllowMultiple = false, Inherited = true)]
	public class AssetReferenceAttribute : Attribute
	{
		public readonly string KeyPropertyName;

		public AssetReferenceAttribute(string fieldName)
		{
			KeyPropertyName = fieldName;
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
		: Asset
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

		public void Reload()
		{
			Assets.Clear();
			if (!File.Exists(FilePath))
				return;
			using (var csv = new CsvReader(new StreamReader(FilePath), AssetManager.CsvConfig))
			{
				Assets.AddRange(csv.GetRecords<TAsset>());
			}

			foreach (var s in Assets)
			{
				s.OnAfterDeserialize();
			}
		}

		public void Save()
		{
			using (var csv = new CsvWriter(new StreamWriter(FilePath), AssetManager.CsvConfig))
			{
				csv.WriteRecords(Assets);
			}
		}
	}
}
