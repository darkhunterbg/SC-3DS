using CsvHelper.Configuration.Attributes;
using DataManager.Build;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace DataManager.Assets
{
    [BinaryData(DataItemType.Sprites)]
    public class UnitAsset : Asset , ITreeViewItem
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

        [DefaultEditor]
        [Binary(BinaryType.AssetRef, 2)]
        [Index(2), TypeConverter(typeof(Asset.AssetConverter))]
        public SpriteAsset Sprite { get; set; }

        [DefaultEditor]
        [Binary(BinaryType.String,32)]
        [Optional]
        public string Name { get; set; } = string.Empty;

        [DefaultEditor]
        [Binary(BinaryType.String, 32)]
        [Optional]
        public string Subtitle { get; set; } = string.Empty;

        [DefaultEditor]
        [Binary(BinaryType.UInt,2)]
        [Optional]
        public int Health { get; set; } = 100;

        [DefaultEditor]
        [Binary(BinaryType.UInt, 2)]
        [Optional]
        public int BarSize { get; set; } = 10;

        [DefaultEditor]
        [Binary(BinaryType.Int, 2)]
        [Optional]
        public int BarOffset { get; set; } = 0;

        [CustomEnumEditor(CustomEnumType.SelectionTypes)]
        [Binary(BinaryType.UInt, 2)]
        [Optional]
        public int SelectonSize { get; set; } = 0;

        [DefaultEditor]
        [Binary(BinaryType.UInt, 2)]
        [Optional]
        public int SelectionOffset { get; set; } = 0;
    }
}
