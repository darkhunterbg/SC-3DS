using CsvHelper;
using CsvHelper.Configuration;
using CsvHelper.TypeConversion;
using DataManager.Build;
using ImGuiNET;

using Microsoft.Xna.Framework.Graphics;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Numerics;
using System.Runtime.InteropServices;
using System.Text;
using System.Threading.Tasks;
using Rectangle = Microsoft.Xna.Framework.Rectangle;
using Color = Microsoft.Xna.Framework.Color;

namespace DataManager.Assets
{
	[BinaryData(DataItemType.VideoClips)]
	public class VideoClip : IDisposable
	{
		[StructLayout(LayoutKind.Sequential)]
		struct Info
		{
			public int width, height, frameCount;
		}

		[StructLayout(LayoutKind.Sequential)]
		struct FrameData
		{
			public IntPtr palette;
			public IntPtr image;
		}

		[DllImport("NativeEditorLib.dll", CallingConvention = CallingConvention.Cdecl)]
		static extern IntPtr SmkOpen([MarshalAs(UnmanagedType.LPStr)] string path);
		[DllImport("NativeEditorLib.dll", CallingConvention = CallingConvention.Cdecl)]
		static extern void SmkClose(IntPtr handle);
		[DllImport("NativeEditorLib.dll", CallingConvention = CallingConvention.Cdecl)]
		static extern Info SmkGetInfo(IntPtr handle);
		[DllImport("NativeEditorLib.dll", CallingConvention = CallingConvention.Cdecl)]
		static extern void SmkDecodeFrame(IntPtr handler, IntPtr pixelData, int texLineSize);
		[DllImport("NativeEditorLib.dll", CallingConvention = CallingConvention.Cdecl)]
		static extern void SmkPlayFirst(IntPtr handler);
		[DllImport("NativeEditorLib.dll", CallingConvention = CallingConvention.Cdecl)]
		static extern bool SmkPlayNext(IntPtr handler);
		[DllImport("NativeEditorLib.dll", CallingConvention = CallingConvention.Cdecl)]
		static extern FrameData SmkGetFrameVideoData(IntPtr handler);

		private IntPtr _handle;

		public bool IsValid => _handle != IntPtr.Zero;

		public Vector2 FrameSize { get; private set; }
		public int FrameCount { get; private set; }
		public int Position { get; private set; }

		public bool IsDone => Position == FrameCount;

		public readonly string FilePath;

		public readonly string RelativePath;

		const int FrameTime = 5;

		[Binary(BinaryType.String, 32)]
		public string Key => RelativePath;
		private GuiTexture _preview = null;
		private int _timer = 0;

		public VideoClip(string path)
		{
			FilePath = path;

			RelativePath = path.Substring(AssetManager.VideoDir.Length);
			RelativePath = RelativePath.Substring(0, RelativePath.Length - 4);

			_handle = SmkOpen(path);
			//if (_handle == IntPtr.Zero)
			//	throw new Exception($"Failed to load Smk file: {path}");
			if (!IsValid) return;

			Info info = SmkGetInfo(_handle);

			FrameSize = new Vector2(info.width, info.height);
			FrameCount = info.frameCount;
			Position = FrameCount;


		}


		public GuiTexture GetPreview()
		{
			if (_preview == null) {
				_preview = new GuiTexture(new Texture2D(AppGame.Device, (int)FrameSize.X, (int)FrameSize.Y));

				Restart();
				Decode(_preview.Texture);
			}

			return _preview;
		}

		public void Dispose()
		{
			if (IsValid)
				SmkClose(_handle);
			_handle = IntPtr.Zero;
			_preview?.Dispose();
			_preview = null;
		}

		public void Restart()
		{
			Position = 0;
			_timer = FrameTime;
			if (IsValid)
				SmkPlayFirst(_handle);
		}

		public bool Next()
		{
			++Position;
			_timer = FrameTime;

			if (IsValid)
				SmkPlayNext(_handle);

			return IsDone;
		}

		private void Decode(Texture2D texture)
		{
			FrameData frameVideoData = SmkGetFrameVideoData(_handle);

			Color[] pixelData = new Color[texture.Width * texture.Height];

			byte[] palette = new byte[3 * 256];
			byte[] image = new byte[(int)FrameSize.X * (int)FrameSize.Y];

			Marshal.Copy(frameVideoData.palette, palette, 0, palette.Length);
			Marshal.Copy(frameVideoData.image, image, 0, image.Length);

			int w = (int)FrameSize.X;
			int h = (int)FrameSize.Y;

			int a = 0;

			for (int i = 0; i < pixelData.Length; ++i) {
				pixelData[i] = Color.Black;
			}

			for (int y = 0; y < (int)h; ++y) {
				for (int x = 0; x < (int)w; ++x) {
					int i = (x + y * texture.Width);


					int index = image[a++];
					index *= 3;


					if (index == 0) {
						pixelData[i] = Color.Transparent;
						continue;
					}

					pixelData[i] = new Color(palette[index], palette[index + 1], palette[index + 2], (byte)255);
				}
			}

			texture.SetData(pixelData);
		}

		public void GUIPreview()
		{
			if (IsValid) {
				ImGui.Text($"Dimensions: {FrameSize.X}x{FrameSize.Y}");
				ImGui.Text($"Frames: {FrameCount}");

				--_timer;

				if (_timer <= 0) {

					if (IsDone) {
						Restart();
					} else {
						Next();
					}

					Decode(GetPreview().Texture);
				}


				var uv = GetPreview().TextureSize;

				int multiplier = 1;
				if (FrameSize.X <= 128 && FrameSize.Y <= 128)
					multiplier = 2;
				if (FrameSize.X <= 32 && FrameSize.Y <= 32)
					multiplier = 4;

				ImGui.Image(GetPreview().GuiImage, uv * multiplier);
			} else {
				ImGui.TextColored(Microsoft.Xna.Framework.Color.Red.ToVec4(), "Failed to load video data.");
			}
		}
	}

	public struct VideoClipRef
	{
		public readonly string Id;

		public readonly VideoClip Clip;


		public bool IsEmpty => string.IsNullOrEmpty(Id);

		public static readonly VideoClipRef None = new VideoClipRef(null);

		public VideoClipRef(VideoClip clip)
		{
			Clip = clip;
			Id = clip?.Key ?? string.Empty;
		}
		public VideoClipRef(string id, VideoClip clip)
		{
			Clip = clip;
			Id = id;
		}

		public override string ToString()
		{
			return Id?.ToString() ?? string.Empty;
		}

		public class CsvConverter : DefaultTypeConverter
		{

			public static VideoClipRef FromString(string text)
			{
				if (string.IsNullOrEmpty(text))
					return VideoClipRef.None;

				VideoClip clip = AppGame.AssetManager.VideoClips.FirstOrDefault(c => c.Key == text);

				return new VideoClipRef(text, clip);
			}
			public override object ConvertFromString(string text, IReaderRow row, MemberMapData memberMapData)
			{
				return FromString(text);
			}
			public override string ConvertToString(object value, IWriterRow row, MemberMapData memberMapData)
			{
				if (value == null)
					return string.Empty;

				return ((VideoClipRef)value).Id.ToString();
			}
		}
	}
}
