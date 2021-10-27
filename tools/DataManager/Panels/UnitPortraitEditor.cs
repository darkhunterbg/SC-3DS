using DataManager.Assets;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace DataManager.Panels
{
	public class UnitPortraitEditor : AssetTreeEditor<UnitPortraitAsset>
	{
		public UnitPortraitEditor() : base("Unit Portrait", "Unit Portraits", "Unit Portraits Editor") { }
	}
}
