using CsvHelper;
using CsvHelper.Configuration;
using CsvHelper.TypeConversion;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace DataManager.Assets
{
	public struct IconRef
	{
		public readonly int Id;
		public readonly ImageFrame Image;

		public static readonly IconRef None = new IconRef(-1, null);

		public IconRef(ImageFrame image)
		{
			Id = image?.FrameIndex ?? -1;
			Image = image;
		}

		public IconRef(int id, ImageFrame image)
		{
			Id = id;
			Image = image;
		}


		public class IconConverter : DefaultTypeConverter
		{
			public override object ConvertFromString(string text, IReaderRow row, MemberMapData memberMapData)
			{
				ImageFrame icon = null;

				if (int.TryParse(text, out int index) && index >= 0)
					icon = AppGame.AssetManager.Icons.Skip(index).FirstOrDefault();

				return new IconRef(index, icon);

			}
			public override string ConvertToString(object value, IWriterRow row, MemberMapData memberMapData)
			{
				return ((IconRef)value).Id.ToString();
			}
		}
	}
}
