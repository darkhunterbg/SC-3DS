
using CsvHelper;
using CsvHelper.Configuration;
using CsvHelper.Configuration.Attributes;
using CsvHelper.TypeConversion;
using DataManager.Build;
using System;
using System.Collections.Generic;
using System.IO;
using System.Linq;
using System.Numerics;
using System.Text;
using System.Threading.Tasks;

namespace DataManager.Assets
{
    [BinaryData(DataItemType.Images)]
    public class ImageListAtlasData
    {
        [Index(0), Name("Image")]
        [Binary(BinaryType.String, 32)]
        public string Name { get; set; }

        [Index(1), Name("FrameOffset")]
        [Binary(BinaryType.UInt, 2)]
        public int FrameOffset { get; set; }

        [Index(2), Name("FrameCount")]
        [Binary(BinaryType.UInt, 2)]
        public int FrameCount { get; set; }

        [Index(3), Name("Width")]
        [Binary(BinaryType.UInt, 2)]
        public int Width { get; set; }

        [Index(4), Name("Height")]
        [Binary(BinaryType.UInt, 2)]
        public int Height { get; set; }

        [Binary(BinaryType.UInt, 2)]
        [Index(5)]
        public int ColorMaskOffset { get; set; }

        [Ignore]
        public ImageList List { get; private set; }

        public ImageListAtlasData(ImageList list)
        {
            List = list;
            Name = list.Key;
            FrameCount = list.Frames.Count;
            Width = (int)list.FrameSize.X;
            Height = (int)list.FrameSize.Y;
            ColorMaskOffset = list.UnitColorOffset;
        }
    }


    public class ImageList
    {
        //public string InfoFilePath { get; private set; }
        private string RelativePath;
        public string Key => RelativePath;
        public string AssetDir { get; private set; }

        public Vector2 FrameSize { get; private set; }

        public List<ImageFrame> Frames { get; private set; } = new List<ImageFrame>();

        public int TakenSpace => Frames.Sum(f => f.TakenSpace);

        public int UnitColorOffset { get; private set; }

        public bool HasUnitColor => UnitColorOffset != 0;

        private ImageList() { }

        public static ImageList FromPng(string pngFile, bool isExtracted = false)
        {
            var i = new ImageList();
            i.AssetDir = Path.GetDirectoryName(pngFile);
            i.RelativePath = pngFile.Substring(AssetManager.AssetsDir.Length);
            i.RelativePath = i.RelativePath.Substring(0, i.RelativePath.Length - 4);


            if(isExtracted ) {
                i.RelativePath = i.RelativePath.Substring("extracted\\".Length);
			}
            using (var image = SixLabors.ImageSharp.Image.Load(pngFile))
            {
                i.FrameSize = new Vector2(image.Width, image.Height);

                i.Frames.Add(new ImageFrame(i, 0)
                {
                    fileName = Path.GetFileName(pngFile),
                    rect = new Microsoft.Xna.Framework.Rectangle(0, 0, image.Width, image.Height)
                });
            }

            return i;
        }

        public static ImageList FromInfoFile(string infoFile)
        {
            var i = new ImageList();

            i.AssetDir = Path.GetDirectoryName(infoFile);
            i.RelativePath = i.AssetDir.Substring(AssetManager.ExtractedAssetsDir.Length);
            //InfoFilePath = infoFile.Substring(AssetManager.AssetsDir.Length);

            string[] info = File.ReadAllLines(infoFile);
            var spl = info[0].Split(' ');
            i.FrameSize = new Vector2(int.Parse(spl[0]), int.Parse(spl[1]));

            foreach (var e in info.Skip(1))
            {
                spl = e.Split(' ');

                Microsoft.Xna.Framework.Rectangle rect = new Microsoft.Xna.Framework.Rectangle();
                rect.X = int.Parse(spl[1]);
                rect.Y = int.Parse(spl[2]);
                rect.Width = int.Parse(spl[3]);
                rect.Height = int.Parse(spl[4]);

                string name = spl[0];

                i.Frames.Add(new ImageFrame(i, i.Frames.Count)
                {
                    fileName = name,
                    rect = rect,
                });
            }

            i.UnitColorOffset = i.Frames.IndexOf(f => f.fileName.StartsWith("cm_"));
            if (i.UnitColorOffset < 0)
                i.UnitColorOffset = 0;

            return i;
        }

        public int GetUnitColorFrameIndex(int frame)
        {
            return frame + UnitColorOffset;
        }

        public ImageFrame GetUnitColorFrame(int frame)
        {
            int i = frame + UnitColorOffset;
            if (i >= Frames.Count)
                return null;
            return Frames[i];
        }


        public ImageFrame GetFrame(int i)
        {
            if (i >= Frames.Count)
                return null;
            return Frames[i];
        }

        public string GetFrameFilePath(int i)
        {
            return $"{AssetDir}\\{Frames[i].fileName}";
        }

    }

    public struct ImageListRef
    {
        public readonly string Key;

        public readonly ImageList Image;

        public static readonly ImageListRef None = new ImageListRef(string.Empty, null);

        public override string ToString()
        {
            return Key;
        }

        public ImageListRef(ImageList image)
        {
            Key = image.Key;
            Image = image;
        }

        public ImageListRef(string key, ImageList image)
        {
            Key = key;
            Image = image;
        }

        public class CsvConverter : DefaultTypeConverter
        {
            public override object ConvertFromString(string text, IReaderRow row, MemberMapData memberMapData)
            {
                if (string.IsNullOrEmpty(text))
                    return ImageListRef.None;

                var i = AppGame.AssetManager.ImageLists.FirstOrDefault(f => f.Key == text);

                return new ImageListRef(text, i);
            }
            public override string ConvertToString(object value, IWriterRow row, MemberMapData memberMapData)
            {
                if (value == null)
                    return string.Empty;

                return ((ImageListRef)value).Key ?? string.Empty;
            }
        }
    }
}
