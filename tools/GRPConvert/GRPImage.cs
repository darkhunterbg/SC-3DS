using System;
using System.Collections.Generic;
using System.Drawing;
using System.IO;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace GRPConvert
{
	public struct Pixel
	{
		public int x, y, color;
	}

	public class GRPFrame
	{
		public int XOffset;
		public int YOffset;

		public int Width, Height;

		public int DataOffset;

		public int[,] Pixels;

		public Bitmap ToBitmap(Palette palette)
		{
			Bitmap b = new Bitmap(Width, Height);
			for (int x = 0; x < Width; ++x)
			{
				for (int y = 0; y < Height; ++y)
				{
					int i = Pixels[x, y];
					if (i == 0)
						continue;
					Color c = palette.Colors[i];
					b.SetPixel(x, y, c);
				}
			}

			return b;
		}

		public bool UsesRemappedColors(Palette palette)
		{
			for (int x = 0; x < Width; ++x)
			{
				for (int y = 0; y < Height; ++y)
				{
					int i = Pixels[x, y];
					if (palette.RemappedIndexes.Contains(i))
						return true;
				}
			}

			return false;
		}

		public Bitmap GenerateMapOfRemappedColors(Palette palette)
		{
			Bitmap b = new Bitmap(Width, Height);
			for (int x = 0; x < Width; ++x)
			{
				for (int y = 0; y < Height; ++y)
				{
					int i = Pixels[x, y];
					if (!(palette.RemappedIndexes.Contains(i)))
						continue;
					b.SetPixel(x, y, Color.White);
				}
			}

			return b;
		}
	}

	public class GRPImage
	{
		public int NumberOfFrames;
		public int MaxWidth, MaxHeight;

		public List<GRPFrame> Frames = new List<GRPFrame>();

		// https://sourceforge.net/p/stratlas/wiki/GRP/

		public GRPImage(string path)
		{
			byte[] data = File.ReadAllBytes(path);

			int i = 0;
			NumberOfFrames = BitConverter.ToInt16(data.AsSpan(i, 2));
			i += 2;
			MaxWidth = BitConverter.ToInt16(data.AsSpan(i, 2));
			i += 2;
			MaxHeight = BitConverter.ToInt16(data.AsSpan(i, 2));
			i += 2;

			for (int f = 0; f < NumberOfFrames; ++f)
			{
				GRPFrame frame = new GRPFrame();
				Frames.Add(frame);

				frame.XOffset = data[i++];
				frame.YOffset = data[i++];

				frame.Width = data[i++];
				frame.Height = data[i++];

				frame.Pixels = new int[frame.Width, frame.Height];


				frame.DataOffset = (int)BitConverter.ToUInt32(data.AsSpan(i, 4));
				i += 4;
				DecodeGRPFrameData(data, frame);
			}
		}


		private void DecodeGRPFrameData(byte[] data, GRPFrame frame)
		{
			int offset = frame.DataOffset;
			List<int> imagRowOffsets = new List<int>();
			for (int i = 0; i < frame.Height; ++i)
			{
				int rowOffset = BitConverter.ToUInt16(data.AsSpan(offset + i * 2, 2));
				imagRowOffsets.Add(rowOffset);
			}


			for (int y = 0; y < frame.Height; ++y)
			{
				offset = frame.DataOffset + imagRowOffsets[y];

				int x = 0;
				while (x < frame.Width)
				{
					byte raw = data[offset++];
					if ((raw & 0x80) == 0)
					{
						if ((raw & 0x40) > 0)
						{

							raw &= 0x3F;
							byte converterdPacket = data[offset++];

							//Set the Player color (Not implemented yet :|
							//covertedPacket = tableof unitColor[ colorbyte+gr_gamenr];

							int operationCounter = raw;
							Pixel p = default;
							p.x = x;

							do
							{
								p.y = y;
								p.color = converterdPacket;
								frame.Pixels[p.x, p.y] = p.color;
								p.x++;
							}
							while (--operationCounter > 0);

							x += raw;
						}
						else
						{
							int operationCounter = raw;
							do
							{
								byte converterdPacket = data[offset++];
								Pixel p = default;
								p.x = x;
								p.y = y;
								p.color = converterdPacket;
								frame.Pixels[p.x, p.y] = p.color;
								x++;
							}
							while (--operationCounter > 0);
						}
					}
					else
					{
						raw &= 0x7F;
						x += raw;
					}
				}
			}
		}
	}
}
