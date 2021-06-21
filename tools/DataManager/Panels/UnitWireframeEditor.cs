using DataManager.Assets;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace DataManager.Panels
{
	public class UnitWireframeEditor :  AssetTreeEditor<UnitWireframeAsset>
	{
		public UnitWireframeEditor() : base("Wireframe", "Wireframes", "Wireframe Editor") { }
	}
}
