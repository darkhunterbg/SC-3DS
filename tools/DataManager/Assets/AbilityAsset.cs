using CsvHelper.Configuration.Attributes;
using DataManager.Build;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace DataManager.Assets
{
    [BinaryData(DataItemType.Abilities)]
    public class AbilityAsset : Asset, ITreeViewItem
    {
        public override string AssetName => Path;
        public override GuiTexture Preview => Icon.Image?.Image;

        [Index(0)]
        [Binary(BinaryType.String, 32)]
        public string Path { get; set; }

        [Optional]
        [Binary(BinaryType.UInt, 4)]
        public AssetId AbilityId { get; set; }

        [DefaultEditor]
        [Optional]
        [Binary(BinaryType.UInt, 2)]
        public IconRef Icon { get; set; } = IconRef.None;


        public AbilityAsset() : base()
        {
            AbilityId = AssetId.New();
        }

        public override Asset Clone()
        {
            AbilityAsset clone = (AbilityAsset)base.Clone();
            clone.AbilityId = AssetId.New();
            return clone;
        }
    }
}
