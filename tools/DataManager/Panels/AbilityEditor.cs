using DataManager.Assets;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace DataManager.Panels
{
    public class AbilityEditor : AssetTreeEditor<AbilityAsset>
    {
        public AbilityEditor() : base("Ability","Abilities", "Ability Editor") { }
    }
}
