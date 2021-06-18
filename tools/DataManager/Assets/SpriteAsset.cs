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
    public class SpriteAsset : Asset, ITreeViewItem
    {
        public override string AssetName => Name;
        public override GuiTexture Preview
        {
            get
            {
                if (Image.Image == null)
                    return null;

                int frame = 0;

                if (IsRotating && Image.Image.Frames.Count > 12)
                    frame += 12;

                return frame >= Image.Image.Frames.Count ? null :
                    Image.Image.Frames[frame].Image;
            }

        }

        [Ignore]
        public string Path { get { return Name; } set { Name = value; } }

        [Index(0)]
        [Binary(BinaryType.String, 32)]
        public string Name { get; set; }

        [Index(1)]
        [DefaultEditor]
        [Binary(BinaryType.ImageRef, 2)]
        public ImageListRef Image { get; set; } = ImageListRef.None;

        [Ignore]
        [Binary(BinaryType.UInt, 2)]
        public int AnimationStart
        {
            get
            {
                if (AnimationCount == 0)
                    return 0;

                var first = Clips.OrderBy(t => t.SortKey).First();

                return AppGame.AssetManager.GetAssetDatabase<SpriteAnimClipAsset>().Assets
                    .IndexOf(first);
            }
        }

        [Ignore]
        [Binary(BinaryType.UInt, 1)]
        public int AnimationCount => Clips.Count;

        [Index(2)]
        [DefaultEditor]
        [Binary(BinaryType.UInt, 1)]
        public bool IsRotating { get; set; }

        [Ignore]
        public bool HasUnitColoring => Image.Image?.HasUnitColor ?? false;

        [Ignore]
        public List<SpriteAnimClipAsset> Clips { get; set; } = new List<SpriteAnimClipAsset>();

        public SpriteAsset() : base() { }

        public override Asset Clone()
        {
            var clone = base.Clone() as SpriteAsset;
            clone.Clips = Clips.Select(s => s.Clone() as SpriteAnimClipAsset).ToList();
            foreach (var c in clone.Clips)
                c.Sprite = clone;

            AppGame.AssetManager.GetAssetDatabase<SpriteAnimClipAsset>().AddRange(clone.Clips);

            return clone;
        }
    }
}
