using CsvHelper.Configuration.Attributes;
using System;
using System.Collections.Generic;
using System.IO;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace DataManager.Assets
{
	public class SpriteAtlasAsset
	{
		private List<SpriteAtlasRecord> records = new List<SpriteAtlasRecord>();

		public string Name { get; private set; }

		public IEnumerable<SpriteAtlasRecord> Atlases => records;

		public void AddRecord(IEnumerable<ImageListAsset> assets)
		{
			records.Add(new SpriteAtlasRecord()
			{
				AtlasName = Name,
				AtlasIndex = records.Count,
				ImageLists = assets.ToList(),
			});
		}


		public void SetRecords(IEnumerable<SpriteAtlasRecord> assets)
		{
			records.AddRange(assets);
		}


		public SpriteAtlasAsset(string name)
		{
			Name = name;
		}
	}

	public class SpriteAtlasRecord
	{
		[Index(0)]
		public string AtlasName { get; set; }

		[Index(1)]
		public int AtlasIndex { get; set; }

		[Ignore]
		public string Name => $"{AtlasName}_{AtlasIndex}";

		[Ignore]
		public string InfoFilePath => $"{ AssetManager.SpriteAtlasDir}{Name}.t3s";

		[Ignore]
		public List<ImageListAsset> ImageLists = new List<ImageListAsset>();

	}
}
