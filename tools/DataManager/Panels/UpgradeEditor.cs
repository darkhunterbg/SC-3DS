using DataManager.Assets;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace DataManager.Panels
{
    public class UpgradeEditor : AssetTreeEditor<UpgradeAsset>
    {
        public UpgradeEditor() : base("Upgade", "Upgrades", "Upgrade Editor") { }
    }
}
