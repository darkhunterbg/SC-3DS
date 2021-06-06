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

		[Ignore]
		public string InfoFilePath => $"{ AssetManager.SpriteAtlasDir}{Name}.t3s";

		public void AddSubAtlas(IEnumerable<ImageFrameAtlasData> assets)
		{
			subAtlases.Add(new SpriteSubAtlas()
			{
				AtlasName = Name,
				AtlasIndex = subAtlases.Count,
				Images = assets.ToList(),
			});

			foreach (var f in subAtlases.Last().Images)
				f.AtlasName = subAtlases.Last().FullName;
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



		public List<ImageFrameAtlasData> Images { get;  set; } = new List<ImageFrameAtlasData>();
	}
}
