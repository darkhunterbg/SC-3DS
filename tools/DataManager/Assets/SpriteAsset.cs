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
		public string Name { get; set; }

		[Index(1)]
		[DefaultEditor]
		[TypeConverter(typeof(ImageListRef.CsvConverter))]
		public ImageListRef Image { get; set; } = ImageListRef.None;

		[Index(2)]
		[DefaultEditor]
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

			return clone;
		}
	}
}
