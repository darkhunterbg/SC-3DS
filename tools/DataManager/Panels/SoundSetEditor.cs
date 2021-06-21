using DataManager.Assets;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace DataManager.Panels
{
	public class SoundSetEditor : AssetTreeEditor<SoundSetAsset>
	{
		public SoundSetEditor() : base("Sound Set", "Sound Sets", "Sound Sets Editor") { }
	}
}
