using DataManager.Assets;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace DataManager.Panels
{
    internal class EffectEditor : AssetTreeEditor<EffectAsset>
    {
        public EffectEditor() : base("Effect","Effects", "Effect Editor")
        {
        }
    }
}
