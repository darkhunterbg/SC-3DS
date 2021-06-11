using CsvHelper.Configuration.Attributes;
using DataManager.Build;
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
		public string InfoFilePath => $"{ AssetManager.SpriteAtlasOutDir}{Name}.t3s";

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

	[BinaryData(DataItemType.Atlases)]
	public class SpriteSubAtlas
	{
		[Index(0),Binary(BinaryType.String, 32)]
		public string AtlasName { get; set; }

		[Index(1), Binary(BinaryType.UInt, 2)]
		public int AtlasIndex { get; set; }

		[Ignore]
		public string FullName => $"{AtlasName}_{AtlasIndex}";

		public List<ImageFrameAtlasData> Images { get;  set; } = new List<ImageFrameAtlasData>();
	}
}
