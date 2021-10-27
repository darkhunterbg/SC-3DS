using CsvHelper.Configuration.Attributes;
using DataManager.Build;
using ImGuiNET;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace DataManager.Assets
{
	[BinaryData(DataItemType.Wireframe)]
	public class UnitWireframeAsset : Asset, ITreeViewItem
	{
		public override string AssetName => Path;
		public override GuiTexture Preview => Icon.Frame?.Image;

		[Index(0)]
		public string Path { get; set; }

		[Optional]
		[ImageFrameEditor("unit\\wirefram\\wirefram")]
		public ImageFrameRef Icon { get; set; } = ImageFrameRef.None;

		[Optional]
		[DefaultEditor]
		[Binary(BinaryType.ImageRef,2)]
		public ImageListRef DetailWireframe { get; set; }

		[Optional]
		[DefaultEditor]
		[Binary(BinaryType.ImageRef, 2)]
		public ImageListRef BuildWireframe { get; set; }

		[Optional]
		[DefaultEditor]
		[Binary(BinaryType.ImageRef, 2)]
		public ImageListRef GroupWireframe { get; set; }

		public override bool HasTooltip => true;

		public override void DrawTooltip()
		{
			if (Icon.Frame != null)
				ImGui.Image(Icon.Frame.Image.GuiImage, Icon.Frame.Image.TextureSize);
		}
	}
}
