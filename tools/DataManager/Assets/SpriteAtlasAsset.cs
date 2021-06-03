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
		private List<SpriteSubAtlas> subAtlases = new List<SpriteSubAtlas>();

		public string Name { get; private set; }

		public IEnumerable<SpriteSubAtlas> SubAtlases => subAtlases;

		public void AddSubAtlas(IEnumerable<ImageListAsset> assets)
		{
			subAtlases.Add(new SpriteSubAtlas()
			{
				AtlasName = Name,
				AtlasIndex = subAtlases.Count,
				ImageLists = assets.ToList(),
			});
		}


		public void SetSubAtlases(IEnumerable<SpriteSubAtlas> assets)
		{
			subAtlases.AddRange(assets);
		}


		public SpriteAtlasAsset(string name)
		{
			Name = name;
		}
	}

	public class SpriteSubAtlas
	{
		[Index(0)]
		public string AtlasName { get; set; }

		[Index(1)]
		public int AtlasIndex { get; set; }

		[Ignore]
		public string FullName => $"{AtlasName}_{AtlasIndex}";

		[Ignore]
		public string InfoFilePath => $"{ AssetManager.SpriteAtlasDir}{FullName}.t3s";

		[Ignore]
		public List<ImageListAsset> ImageLists { get; set; } = new List<ImageListAsset>();


		public List<SpriteSheetAsset> GenerateSpriteSheets()
		{
			List<SpriteSheetAsset> result = new List<SpriteSheetAsset>();
			for(int i = 0; i < ImageLists.Count; ++i)
			{
				result.Add(new SpriteSheetAsset(this, i));
			}
			return result;
		}
	}
}
