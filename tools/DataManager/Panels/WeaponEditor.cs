using DataManager.Assets;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace DataManager.Panels
{
	public class WeaponEditor : AssetTreeEditor<WeaponAsset>
	{
		public WeaponEditor() : base("Weapon", "Weapons", "Weapon Editor") { }
	}
}
