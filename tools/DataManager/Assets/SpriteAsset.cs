using CsvHelper.Configuration.Attributes;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace DataManager.Assets
{
	public class SpriteAsset : Asset, ITreeViewItem
	{
		public override string AssetName => Name;
		public override GuiTexture Preview
		{
			get
			{
				if (Image.Key == null)
					return null;

				int frame = 0;

				if (IsRotating && Image.Image.Frames.Count < 12)
					frame += 12;

				return Image.Image.Frames.Count >= frame ? null :
					Image.Image.Frames[frame].Image;
			}

		}

		[Ignore]
		public string Path { get { return Name; } set { Name = value; } }

		[Index(0)]
		[DefaultEditor]
		public string Name { get; set; }

		[Index(1)]
		[DefaultEditor]
		[TypeConverter(typeof(ImageListRef.CsvConverter))]
		public ImageListRef Image { get; set; } = ImageListRef.None;

		[Index(2)]
		[DefaultEditor]
		public bool IsRotating { get; set; }

		[Index(3)]
		[DefaultEditor]
		public bool HasUnitColoring { get; set; }

		public SpriteAsset() : base() {
		}
	}
}
