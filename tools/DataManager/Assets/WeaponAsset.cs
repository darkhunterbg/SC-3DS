﻿using CsvHelper.Configuration.Attributes;
using DataManager.Build;
using ImGuiNET;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace DataManager.Assets
{
	[BinaryData(DataItemType.Weapons)]
	public class WeaponAsset : Asset, ITreeViewItem
	{
		public override string AssetName => Path;
		public override GuiTexture Preview => Icon.Image?.Image;

		[Index(0)]
		public string Path { get; set; }

		[Section("Art")]

		[DefaultEditor]
		[Optional]
		[Binary(BinaryType.UInt, 2)]
		public IconRef Icon { get; set; } = IconRef.None;

		[Section("Text")]

		[DefaultEditor]
		[Optional]
		[Binary(BinaryType.String, 32)]
		public string Name { get; set; } = string.Empty;

		[DefaultEditor]
		[Optional]
		[Binary(BinaryType.UInt, 2)]
		public int Attacks { get; set; } = 1;

		public override bool HasTooltip => true;

		public override void DrawTooltip()
		{
			if (Icon.Image != null)
				ImGui.Image(Icon.Image.Image.GuiImage, Icon.Image.Image.TextureSize);

			ImGui.SameLine();

			ImGui.Text(Name);
		}
	}
}
