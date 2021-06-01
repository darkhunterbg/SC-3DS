using Microsoft.Xna.Framework;
using Microsoft.Xna.Framework.Graphics;
using System;
using System.Collections.Generic;
using System.IO;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace DataManager.Assets
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

		static int MinTexSize = 128;

		private Texture2D NewTexture()
		{
			int w = Width / MinTexSize;
			if (Width % MinTexSize != 0)
				w += 1;
			int h = Height / MinTexSize;
			if (Height % MinTexSize != 0)
				h += 1;
			return new Texture2D(AppGame.Device, w * MinTexSize, h * MinTexSize);
		}

		public void DrawTo(Texture2D texture, Palette palette)
		{
			int w = Math.Min(Width, texture.Width);
			int h = Math.Min(Height, texture.Height);
			Color[] data = new Color[w * h];
			for (int x = 0; x < w; ++x)
			{
				for (int y = 0; y < h; ++y)
				{
					int i = Pixels[x, y];
					if (i == 0)
						continue;

					Color c = palette.Colors[i];
					data[x + y * w] = c;
				}
			}

			texture.SetData(0, new Rectangle(0, 0, w, h), data, 0, data.Length);
		}

		public Texture2D ToTexture(Palette palette)
		{
			Texture2D b = NewTexture();
			Color[] data = new Color[Width * Height];
			for (int x = 0; x < Width; ++x)
			{
				for (int y = 0; y < Height; ++y)
				{
					int i = Pixels[x, y];
					if (i == 0)
						continue;
					Color c = palette.Colors[i];
					data[x + y * Width] = c;
				}
			}

			b.SetData(0, new Rectangle(0, 0, Width, Height), data, 0, data.Length);

			return b;
		}

		public List<Texture2D> GenerateWireframeImages(Palette palette)
		{
			List<int> wireFrameColors = new List<int>();

			for (int x = 0; x < Width; ++x)
			{
				for (int y = 0; y < Height; ++y)
				{
					int i = Pixels[x, y];
					if (i == 0)
						continue;

					if (!wireFrameColors.Contains(i) && i > 200)
						wireFrameColors.Add(i);
				}
			}

			List<Texture2D> images = new List<Texture2D>();

			Texture2D b = new Texture2D(AppGame.Device, Width, Height);
			images.Add(b);
			Color[] data = new Color[Width * Height];
			for (int x = 0; x < Width; ++x)
			{
				for (int y = 0; y < Height; ++y)
				{
					int i = Pixels[x, y];
					if (i == 0 || wireFrameColors.Contains(i))
						continue;

					Color c = palette.Colors[i];
					data[x + y * Width] = c;
				}
			}

			b.SetData(data);

			foreach (int j in wireFrameColors)
			{
				b = new Texture2D(AppGame.Device, Width, Height);
				images.Add(b);
				data = new Color[Width * Height];
				for (int x = 0; x < Width; ++x)
				{
					for (int y = 0; y < Height; ++y)
					{
						int i = Pixels[x, y];

						if (i != j)
							continue;
						data[x + y * Width] = Color.White;

					}
				}

				b.SetData(data);
			}
			return images;

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

		public Texture2D GenerateMapOfRemappedColors(Palette palette)
		{
			Texture2D b = new Texture2D(AppGame.Device, Width, Height);
			var data = new Color[Width * Height];
			for (int x = 0; x < Width; ++x)
			{
				for (int y = 0; y < Height; ++y)
				{
					int i = Pixels[x, y];
					if (!(palette.RemappedIndexes.Contains(i)))
						continue;
					data[x + y * Width] = palette.Remapped[i];
				}
			}
			b.SetData(data);

			return b;
		}
	}

	public class GRPImage : IDisposable
	{
		public int NumberOfFrames;
		public int MaxWidth, MaxHeight;

		public List<GRPFrame> Frames = new List<GRPFrame>();

		public GRPFrame Preview => Frames[0];

		public readonly string Path;

		// https://sourceforge.net/p/stratlas/wiki/GRP/

		public GRPImage(string path)
		{
			Path = path;

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


		public Palette GetDefaultPalette()
		{
			if (Path.Contains("cmdicons"))
				return AppGame.AssetManager.Palettes["CmdIcons"];

			if (Path.Contains("thingy"))
				return AppGame.AssetManager.Palettes["Orange"];

			if (Path.Contains("shad"))
				return AppGame.AssetManager.Palettes["Shadow"];

			return AppGame.AssetManager.Palettes["Units"];
		}

		public void LoadPreview(Texture2D src)
		{
			Palette p = GetDefaultPalette();
			Preview.DrawTo(src, p);
		}

		public void Dispose()
		{
			
		}
	}
}
