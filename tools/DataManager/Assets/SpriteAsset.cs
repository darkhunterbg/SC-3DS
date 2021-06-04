using CsvHelper.Configuration.Attributes;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace DataManager.Assets
{
	public class SpriteAsset
	{
		private ImageAsset _img;
		[Ignore]
		public ImageAsset Image
		{
			get
			{
				return _img;
			}
			set
			{
				_img = value;
				ImageName = _img?.SpriteSheetName ?? string.Empty;
			}
		}

		[Index(0)]
		public string Name { get; set; } = string.Empty;

		[Index(1), Name("Image")]
		public string ImageName { get; set; } = string.Empty;

		[Index(2)]
		public int BarSize { get; set; }

		[Index(3), Name("SelType")]
		public int SelectionType { get; set; }

		[Index(4), Name("SelY")]
		public int SelectionY { get; set; }

		static int id = 0;

		[Ignore]
		public readonly int Id;

		public SpriteAsset()
		{
			Id = ++id;
		}

		public SpriteAsset(ImageAsset asset)
		{
			Id = ++id;
			Image = asset;
		}
		public void OnAfterDeserialize(ImageAsset asset)
		{
			Image = asset;
		}
	}
}
