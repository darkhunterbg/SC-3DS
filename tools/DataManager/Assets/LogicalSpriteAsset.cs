using CsvHelper.Configuration.Attributes;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace DataManager.Assets
{
	public class LogicalSpriteAsset
	{
		private LogicalImageAsset _img;
		[Ignore]
		public LogicalImageAsset Image
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

		[Index(3)]
		[Name("BarY")]
		public int BarOffset { get; set; }

		[Index(4)]
		[Name("SelType")]
		public int SelectionType { get; set; }

		[Index(5)]
		[Name("SelY")]
		public int SelectionOffset { get; set; }


		static int id = 0;

		[Ignore]
		public readonly int Id;

		public LogicalSpriteAsset()
		{
			Id = ++id;
		}

		public LogicalSpriteAsset(LogicalSpriteAsset copy)
		{
			Id = ++id;
			Image = copy.Image;
			Name = copy.Name;
			BarSize = copy.BarSize;
			BarOffset = copy.BarOffset;
			SelectionType = copy.SelectionType;
			SelectionOffset = copy.SelectionOffset;
		}

		public LogicalSpriteAsset(LogicalImageAsset asset)
		{
			Id = ++id;
			Image = asset;
		}
		public void OnAfterDeserialize(LogicalImageAsset asset)
		{
			Image = asset;
		}
	}
}
