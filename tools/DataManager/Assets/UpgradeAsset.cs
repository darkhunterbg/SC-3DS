using CsvHelper.Configuration.Attributes;
using DataManager.Build;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace DataManager.Assets
{
    [BinaryData(DataItemType.Upgrades)]
    public class UpgradeAsset : Asset, ITreeViewItem
    {
        public override string AssetName => Path;
        public override GuiTexture Preview => Icon.Image?.Image;

        [Index(0)]
        [Binary(BinaryType.String, 32)]
        public string Path { get; set; }

        [Optional]
        [Binary(BinaryType.UInt, 4)]
        public AssetId UpdateId { get; set; }

        [DefaultEditor]
        [Optional]
        [Binary(BinaryType.UInt, 2)]
        public IconRef Icon { get; set; } = IconRef.None;


        public UpgradeAsset() : base()
        {
            UpdateId = AssetId.New();
        }

        public override Asset Clone()
        {
            UpgradeAsset clone = (UpgradeAsset)base.Clone();
            clone.UpdateId = AssetId.New();
            return clone;
        }
    }
}
