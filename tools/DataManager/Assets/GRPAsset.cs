using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace DataManager.Assets
{
	public class GRPAsset
	{
		public string Path;
		public string DisplayName;

		public GRPAsset(string file)
		{
			Path = file;
			DisplayName = file.Substring(AssetManager.RawAssetDir.Length);
		}
	}

}
