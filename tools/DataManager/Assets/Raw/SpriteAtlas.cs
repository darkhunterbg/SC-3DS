using CsvHelper.Configuration.Attributes;
using System;
using System.Collections.Generic;
using System.IO;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace DataManager.Assets
{
	public class SpriteAtlas 
	{
		private List<SpriteSubAtlas> subAtlases = new List<SpriteSubAtlas>();

		public string Name { get; private set; }

		public IEnumerable<SpriteSubAtlas> SubAtlases => subAtlases;

		public void AddSubAtlas(IEnumerable<ImageList> assets)
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


		public SpriteAtlas(string name)
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
		public List<ImageList> ImageLists { get; set; } = new List<ImageList>();


		public List<SpriteSheetAsset> GenerateSpriteSheets()
		{
			List<SpriteSheetAsset> result = new List<SpriteSheetAsset>();
			for(int i = 0; i < ImageLists.Count; ++i)
			{
				result.Add(new SpriteSheetAsset(this, i));
			}
			return result;
		}

		public ImageList GetImageListAtOffset(int offset)
		{
			int i = 0;
			while (offset > 0)
			{
				offset -= ImageLists[i].Frames.Count;
				++i;
			}

			return ImageLists[i];
		}
	}
}
