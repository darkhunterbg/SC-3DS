using ImGuiNET;
using Microsoft.Xna.Framework.Graphics;
using System;
using System.Collections.Generic;
using System.IO;
using System.Linq;
using System.Numerics;
using System.Runtime.InteropServices;
using Color = Microsoft.Xna.Framework.Color;
using Rectangle = Microsoft.Xna.Framework.Rectangle;


namespace DataManager.Assets
{
	public interface IRawImage : IDisposable
	{
		string Path { get; }

		void GUIPreview(GuiTexture src);
	}

	[StructLayout(LayoutKind.Sequential, Pack = 1)]
	public struct PCXHeander
	{

		public byte Identifier;        /* PCX Id Number (Always 0x0A) */
		public byte Version;           /* Version Number */
		public byte Encoding;          /* Encoding Format */
		public byte BitsPerPixel;      /* Bits per Pixel */
		public ushort XStart;            /* Left of image */
		public ushort YStart;            /* Top of Image */
		public ushort XEnd;              /* Right of Image */
		public ushort YEnd;              /* Bottom of image */
		public ushort HorzRes;           /* Horizontal Resolution */
		public ushort VertRes;           /* Vertical Resolution */

		[MarshalAs(UnmanagedType.ByValArray, SizeConst = 48)]
		public byte[] Palette;       /* 16-Color EGA Palette */
		public byte Reserved1;         /* Reserved (Always 0) */
		public byte NumBitPlanes;      /* Number of Bit Planes */
		public ushort BytesPerLine;      /* Bytes per Scan-line */
		public ushort PaletteType;       /* Palette Type */
		public ushort HorzScreenSize;    /* Horizontal Screen Size */
		public ushort VertScreenSize;    /* Vertical Screen Size */
		[MarshalAs(UnmanagedType.ByValArray, SizeConst = 54)]
		public byte[] Reserved2;     /* Reserved (Always 0) */
	}

	public class PCXImage : IRawImage
	{
		public string Path { get; }

		public Palette Palette { get; }

		public List<byte[]> ScanLines { get; } = new List<byte[]>();

		public int Width { get; private set; }
		public int Height { get; private set; }

		public bool IsPaletteFormat => ScanLines.Count == 1;

		public PCXImage(string path)
		{
			Path = path;


			using (FileStream fs = File.OpenRead(path)) {
				byte[] binHeader = new byte[128];
				fs.Read(binHeader, 0, binHeader.Length);

				GCHandle handle = GCHandle.Alloc(binHeader, GCHandleType.Pinned);
				PCXHeander header = (PCXHeander)Marshal.PtrToStructure(handle.AddrOfPinnedObject(), typeof(PCXHeander));
				handle.Free();

				// https://gist.github.com/warmwaffles/64d1a8c677997fc8c3df4fa7364d33cc

				int ImageWidth = header.XEnd - header.XStart + 1; /* Width of image in pixels */
				int ImageHeight = header.YEnd - header.YStart + 1; /* Length of image in scan lines */
				int ScanLineLength = header.NumBitPlanes * header.BytesPerLine;

				Width = ImageWidth;
				Height = ImageHeight;

				int LinePaddingSize = ((header.BytesPerLine * header.NumBitPlanes) *
				  (8 / header.BitsPerPixel)) - ((header.XEnd - header.XStart) + 1);

				for (int l = 0; l < ImageHeight; ++l) {
					int total = 0;
					int index = 0;
					byte[] buffer = new byte[ScanLineLength];

					ScanLines.Add(buffer);

					do {
						int i = fs.ReadByte();                  /* Get next byte */
						byte b = (byte)i;

						int runcount = 0;
						byte runvalue = 0;

						if ((b & 0xC0) == 0xC0) {        /* 2-byte code */
							runcount = b & 0x3F;              /* Get run count */
							runvalue = (byte)fs.ReadByte();             /* Get pixel value */
						} else                                   /* 1-byte code */
						  {
							runcount = 1;                        /* Run count is one */
							runvalue = b;                     /* Pixel value */
						}
						/* Write the pixel run to the buffer */
						for (total += runcount; runcount > 0 && index < buffer.Length; runcount--, index++)
							buffer[index] = runvalue;
					} while (index < buffer.Length);

					fs.Position += LinePaddingSize;
				}

				byte pal = (byte)fs.ReadByte();

				if (pal == 0x0C) {
					byte[] palData = new byte[(header.BitsPerPixel / 8) * 3 * 256];
					fs.Read(palData, 0, palData.Length);

					Palette = new Palette(palData);

					// Voodomagic for fixing palette on UI only
					string name = System.IO.Path.GetFileNameWithoutExtension(Path);
					if (name.StartsWith("p") && !IsPaletteFormat) {
						for (int i = 0; i < Palette.Colors.Count; ++i) {
							if (Palette.Colors[i].ToVector3().Length() * 255 < 12)
								Palette.Colors[i] = new Color(Palette.Colors[i], 160);

						}
					}
				} else {
					throw new Exception("Unknown PCX format! Palette not found!");
				}

			}
		}

		public void Dispose()
		{
		}


		public void LoadPreview(Texture2D texture)
		{
			int w = Math.Min(Width, texture.Width);
			int h = Math.Min(Height, texture.Height);
			Color[] data = new Color[w * h];
			for (int x = 0; x < w; ++x) {
				for (int y = 0; y < h; ++y) {
					int i = ScanLines[y][x];
					if (i == 0)
						continue;

					Color c = Palette.Colors[i];
					data[x + y * w] = c;
				}
			}

			texture.SetData(0, new Rectangle(0, 0, w, h), data, 0, data.Length);
		}

		public void GUIPreview(GuiTexture src)
		{
			LoadPreview(src.Texture);

			ImGui.Text($"Dimensions: {Width}x{Height}");
			if (IsPaletteFormat) ImGui.Text($"Detected as palette");

			Vector2 uv = new Vector2(Width, Height);
			Vector2 size = new Vector2(Width, Width);
			int multiplier = 1;
			if (size.X <= 128 && size.Y <= 128)
				multiplier = 2;
			if (size.X <= 64 && size.Y <= 64)
				multiplier = 4;
			//if (size.X <= 32 && size.Y <= 32)
			//	multiplier = 8;

			ImGui.Image(src.GuiImage, uv * multiplier, Vector2.Zero, uv / src.Texture.Width);

			var stats = ScanLines.SelectMany(s => s).Where(s => s != 0)
				.GroupBy(c => c).OrderByDescending(c => c.Count()).Take(5);

			if (Width * Height > 0) {
				ImGui.Text("Top Colors:");

				foreach (var s in stats) {


					ImGui.ColorButton(string.Empty, Palette.Colors[s.Key].ToVec4());
					ImGui.SameLine();
					int percent = s.Count() * 1000 / (Width * Height);
					ImGui.Text($"[{s.Key}]: {((float)percent / 10.0f)}%%");
				}
			}
		}

		public Texture2D ToTexture()
		{
			Texture2D b = new Texture2D(AppGame.Device, Width, Height);
			LoadPreview(b);
			return b;
		}
	}
}
