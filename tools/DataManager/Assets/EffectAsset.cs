using CsvHelper.Configuration.Attributes;
using DataManager.Build;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace DataManager.Assets
{
    [BinaryData(DataItemType.Effects)]
    public class EffectAsset : Asset, ITreeViewItem
    {
        public override string AssetName => Path;
        public override GuiTexture Preview => Icon.Image?.Image;

        [Index(0)]
        [Binary(BinaryType.String, 32)]
        public string Path { get; set; }

        [DefaultEditor]
        [Index(1)]
        public IconRef Icon { get; set; } = IconRef.None;

        [DefaultEditor]
        [Binary(BinaryType.String, 32)]
        public string Name { get; set; } = string.Empty;
    }
}
