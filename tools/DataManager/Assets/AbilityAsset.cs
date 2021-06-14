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

        [DefaultEditor]
        [Index(1), TypeConverter(typeof(IconRef.IconConverter))]
        public IconRef Icon { get; set; } = IconRef.None;

        [Ignore]
        [Binary(BinaryType.UInt, 2)]
        public int _IconId => Icon.Id;
    }
}
