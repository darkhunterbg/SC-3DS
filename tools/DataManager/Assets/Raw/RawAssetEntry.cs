using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace DataManager.Assets
{
	public class RawAssetEntry
	{
		public string Path;
		public string DisplayName;

		public bool IsGRP => Path.EndsWith(".grp");
		public bool IsPCX => Path.EndsWith(".pcx");

		public RawAssetEntry(string file)
		{
			Path = file;
			DisplayName = file.Substring(AssetManager.StarcraftAssetDir.Length);
		}

		public IRawImage LoadAsImage()
		{
			if (IsGRP) return new GRPImage(Path);
			if (IsPCX) return new PCXImage(Path);

			return null;
		}
	}

}
