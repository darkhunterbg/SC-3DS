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
		public string InfoFile { get; private set; }
		public string Name { get; protected set; }

		public SpriteAtlasAsset(string path)
		{
			InfoFile = path;
			Name = Path.GetFileNameWithoutExtension(InfoFile);
		}
	}
}
