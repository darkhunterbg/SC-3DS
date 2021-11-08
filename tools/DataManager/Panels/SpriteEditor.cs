using DataManager.Assets;
using DataManager.Widgets;
using ImGuiNET;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Numerics;
using System.Text;
using System.Threading.Tasks;
using Rectangle = Microsoft.Xna.Framework.Rectangle;
using Color = Microsoft.Xna.Framework.Color;
using Microsoft.Xna.Framework.Graphics;
using DataManager.Gameplay;

namespace DataManager.Panels
{
	public class SpriteEditor : IGuiPanel
	{

		class AnimInstructionView
		{
			public AnimClipInstruction Instruction;

			public object[] Parameters;

			public bool Valid;

			public string Text = string.Empty;

			public bool IsLabel = false;
		}


		public string WindowName => "Sprite Editor";

		private SpriteView spriteView = new SpriteView() { ShowCollider = true };


		private SpriteAsset prevSelection = null;
		private SpriteAsset Selected => treeView.Selected as SpriteAsset;
		private SpriteAnimClipAsset SelectedAnimClip => spriteView.Clip;

		private TreeView treeView = new TreeView("tree")
		{
			DataSource = AppGame.AssetManager.GetAssets<SpriteAsset>(),
			ItemName = "Sprite",
			RootName = "Sprites"
		};

		private TablePropertyEditor propertyEditor = new TablePropertyEditor("sprite.propertyEditor");

		public SpriteEditor()
		{
			treeView.NewItemAction = NewItem;
			treeView.DeleteItemAction = DeleteItem;

			AppGame.AssetManager.OnAssetsReloaded += () => treeView.DataSource = AppGame.AssetManager.GetAssets<SpriteAsset>();
		}

		private ITreeViewItem NewItem(ITreeViewItem item)
		{
			var asset = item as SpriteAsset;
			return AppGame.AssetManager.GetAssetDatabase<SpriteAsset>().New(asset);
		}

		private void DeleteItem(ITreeViewItem item)
		{
			var asset = item as SpriteAsset;
			AppGame.AssetManager.GetAssetDatabase<SpriteAsset>().Delete(asset);
			AppGame.AssetManager.GetAssetDatabase<SpriteAnimClipAsset>().DeleteAll(a => asset.Clips.Contains(a));
		}

		bool _clipModified = false;
		bool _selectedClipChanged = false;

		public void Draw(Vector2 client)
		{

			_clipModified = false;


			ImGui.Columns(4, "spriteeditor");

			ImGui.BeginChild("##items");
			{
				treeView.Draw();

				if (prevSelection != Selected) {
					prevSelection = Selected;

					propertyEditor.EditingItem = treeView.Selected;
					spriteView.Sprite = treeView.Selected as SpriteAsset;
				}
			}
			ImGui.EndChild();

			ImGui.NextColumn();

			ImGui.BeginChild("##script");
			{
				propertyEditor.Draw(new Vector2(0, 100));
				ImGui.Separator();
				DrawScriptEditor();
			}
			ImGui.EndChild();

			ImGui.NextColumn();

			ImGui.BeginChild("##anim");
			{
				spriteView.Draw(ImGui.GetColumnWidth());
				_selectedClipChanged = spriteView.ClipChanged;
				ImGui.NewLine();
			}
			ImGui.EndChild();

			ImGui.NextColumn();

			ImGui.BeginChild("assetview");
			{
				DrawSpriteFrames(ImGui.GetColumnWidth());
			}
			ImGui.EndChild();



			DrawIScriptImportModal();


			if (treeView.ItemModified)
				AppGame.AssetManager.SaveAllAssets();
			else {
				if (propertyEditor.Changed) {
					AppGame.AssetManager.GetAssetDatabase<SpriteAsset>().Save();
					//clipModified = true;
				}
				if (_clipModified) {
					AppGame.AssetManager.GetAssetDatabase<SpriteAnimClipAsset>().Save();
					AppGame.AssetManager.GetAssetDatabase<SpriteAsset>().Save();
				}
			}

		}

		private void DrawSpriteFrames(float width)
		{
			if (Selected != null && Selected.Image.Image != null) {
				float x = 0;

				float scale = 1;
				if (Selected.Image.Image.FrameSize.X <= 64 || Selected.Image.Image.FrameSize.Y <= 90)
					scale *= 2;

				if (Selected.Image.Image.FrameSize.X <= 16 || Selected.Image.Image.FrameSize.Y <= 20)
					scale *= 2;

				int i = 0;

				foreach (var frame in Selected.Image.Image.Frames) {
					if (Selected.Image.Image.HasUnitColor && Selected.Image.Image.UnitColorOffset <= frame.FrameIndex)
						break;

					++i;
					x += frame.Size.X * scale;
					if (x >= width - 140) {
						x = frame.Size.X * scale;
						i = 1;
					} else {
						if (!Selected.IsRotating || i <= 17) {
							ImGui.SameLine();
						} else {
							i = 1;
							x = frame.Size.X * scale;
						}
					}

					if (frame.FrameIndex == spriteView.AnimData.State.FrameIndex) {
						ImGui.Image(frame.Image.GuiImage, frame.Size * scale - Vector2.One * 2, Vector2.Zero, Vector2.One, Vector4.One, Vector4.One);
					} else
						ImGui.Image(frame.Image.GuiImage, frame.Size * scale);
					if (ImGui.IsItemHovered())
						AppGame.Gui.HoverObject = frame;
				}
			}
		}

		List<AnimInstructionView> instructions = new List<AnimInstructionView>();
		string buffer = string.Empty;

		string _modalBuffer = string.Empty;
		bool _modalOpen = false;

		private void DrawIScriptImportModal()
		{
			if (!_modalOpen)
				return;

			ImGui.OpenPopup("SpriteEditor.ImportIScript");

		
			if (!ImGui.BeginPopupModal("SpriteEditor.ImportIScript", ref _modalOpen, ImGuiWindowFlags.AlwaysAutoResize | ImGuiWindowFlags.NoTitleBar))
				return;


			ImGui.Text("Place sprite iScript Here");

			ImGui.InputTextMultiline("text", ref _modalBuffer, 10000,new Vector2(800,600), ImGuiInputTextFlags.Multiline | ImGuiInputTextFlags.AllowTabInput);

			if (ImGui.Button("Cancel")) {
				_modalOpen = false;
			}
			ImGui.SameLine();
			if (ImGui.Button("Ok")) {
				IScriptParser parsers = new IScriptParser();
				parsers.Parse(_modalBuffer);
				{
					AppGame.AssetManager.GetAssetDatabase<SpriteAnimClipAsset>()
						.Delete(Selected.Clips);
				}
				Selected.Clips.Clear();
				_selectedClipChanged = true;
				_clipModified = true;

				foreach (var anim in parsers.Animations) {

					var clip =  new SpriteAnimClipAsset(Selected, anim.Key);
					AppGame.AssetManager.GetAssetDatabase<SpriteAnimClipAsset>().Add(clip);
					clip.SetInstructions(anim.Value.GetInstructions());
					Selected.Clips.Add(clip);
				}

				_modalOpen = false;
			}

			ImGui.EndPopup();
		}

		private void DrawScriptEditor()
		{
			if (Selected == null)
				return;

			if (ImGui.Button("Generate from iScript")) {
				_modalBuffer = string.Empty;
				_modalOpen = true;
			}

			if (_selectedClipChanged) {
				buffer = SelectedAnimClip == null ? string.Empty : string.Join('\n', SelectedAnimClip.Instructions);
				instructions = ParseClipInstructions(buffer);

			}

			var clip = SelectedAnimClip;

			ImGui.Columns(2, "script");

			float start = ImGui.GetCursorPosY();

			ImGui.SetCursorPosY(start + 4);

			int n = 0;
			for (int i = 0; i < instructions.Count; ++i) {
				if (i == spriteView.AnimData.State.InstructionId) {
					Vector2 p = ImGui.GetCursorScreenPos();
					p.X -= 10;
					Vector2 s = new Vector2(ImGui.GetColumnWidth(), 25);

					Color c = Color.Yellow;
					c.A = 50;
					ImGui.GetForegroundDrawList().AddRectFilled(p, p + s, c.PackedValue, 2);
				}

				if (i == spriteView.AnimData.BreakpointAt) {
					Vector2 p = ImGui.GetCursorScreenPos();
					//p.X -= 10;
					p += new Vector2(12, 12);
					//Vector2 s = new Vector2(ImGui.GetColumnWidth(), 25);

					Color c = Color.Red;
					c.A = 100;
					ImGui.GetForegroundDrawList().AddCircleFilled(p, 14, c.PackedValue);
				}

				if (!instructions[i].IsLabel) {
					ImGui.TextDisabled($"{n++:D2}");
				} else {
					ImGui.TextDisabled(string.Empty);
				}
				if (ImGui.IsItemClicked(ImGuiMouseButton.Left)) {
					spriteView.AnimData.TimeDelay = 0;
					spriteView.AnimData.State.SetInstruction(i);
				}
				if (ImGui.IsItemClicked(ImGuiMouseButton.Right)) {
					if (i == spriteView.AnimData.BreakpointAt)
						spriteView.AnimData.BreakpointAt = int.MinValue;
					else
						spriteView.AnimData.BreakpointAt = i;
				}
				ImGui.SetCursorPosY(ImGui.GetCursorPosY() - 4);
			}

			ImGui.SetCursorPos(new Vector2(40, start));

			if (ImGui.InputTextMultiline(string.Empty, ref buffer, 1024, new Vector2(-1, -1),
				ImGuiInputTextFlags.Multiline)) {
				if (string.IsNullOrWhiteSpace(buffer)) {
					instructions.Clear();
					if (clip != null) {
						_clipModified = true;
						AppGame.AssetManager.GetAssetDatabase<SpriteAnimClipAsset>().Delete(clip);
						Selected.Clips.Remove(clip);
						clip = null;
					}
				} else {
					instructions = ParseClipInstructions(buffer);
					_clipModified = true;

					if (clip == null) {
						clip = new SpriteAnimClipAsset(Selected, spriteView.SelectedAnimType);
						AppGame.AssetManager.GetAssetDatabase<SpriteAnimClipAsset>().Add(clip);
						Selected.Clips.Add(clip);
					}

					UpdateClipInstructions(clip);
				}

			}
			int j = 0;

			ImGui.SetCursorPos(new Vector2(40, start + 4));
			foreach (var instr in instructions) {
				if (!instr.Valid) {
					Vector2 p = ImGui.GetCursorScreenPos();
					Vector2 s = new Vector2(ImGui.CalcItemWidth(), ImGui.CalcTextSize(instr.Text).Y);
					p.Y += j * s.Y;

					Color c = Color.Red;
					c.A = 100;
					ImGui.GetForegroundDrawList().AddRectFilled(p, p + s, c.PackedValue, 2);
				}

				if (instr.IsLabel) {
					Vector2 p = ImGui.GetCursorScreenPos();
					Vector2 s = new Vector2(ImGui.CalcItemWidth(), ImGui.CalcTextSize(instr.Text).Y);
					p.Y += j * s.Y;

					Color c = Color.Black;
					c.A = 100;
					ImGui.GetForegroundDrawList().AddRectFilled(p, p + s, c.PackedValue, 2);
				}

				++j;
			}


			ImGui.NextColumn();

			if (clip != null) {
				int instrId = 0;

				foreach (var instr in instructions) {
					ImGui.PushID(++instrId);
					if (instr.Instruction == null) {
						ImGui.Text(string.Empty);
					} else {
						ImGui.Text(instr.Instruction.Instruction);
						int i = 0;
						foreach (var param in instr.Instruction.Parameters) {
							ImGui.SameLine();

							if (instr.Parameters?.Length > i && instr.Parameters[i] is Asset asset) {
								EditorFieldDrawer.AssetReadonly("", asset);
							} else {
								ImGui.TextDisabled(param.ToString());
							}



							++i;
						}
					}

					ImGui.PopID();
					ImGui.SetCursorPosY(ImGui.GetCursorPosY() - 4);
				}

				//if (instructionsEdited)
				//{
				//	UpdateClipInstructions(clip);
				//	buffer = string.Join('\n', clip.Instructions);
				//}
			}
		}

		#region Commented
		/*private bool DrawInstructionEditor(AnimInstructionView instr)
		{
			int paramId = 0;
			bool edited = false;

			if (instr.Parameters != null)
				foreach (var param in instr.Parameters)
				{
					ImGui.SameLine();
					ImGui.SetCursorPosX(ImGui.GetCursorPosX() + 10);
					ImGui.PushID(paramId);
					ImGui.SetNextItemWidth(100);
					object newVal = EditorFieldDrawer.Object(string.Empty, param, out bool changed);
					if (changed)
					{
						instr.Parameters[paramId] = newVal;
						edited = true;
					}
					++paramId;
					ImGui.PopID();

					if (edited)
					{
						instr.Text = instr.Instruction.StringSerialize(instr.Parameters);
					}
				}

			return edited;
		}
		*/
		#endregion

		private void UpdateClipInstructions(SpriteAnimClipAsset clip)
		{
			if (!instructions.Any(s => s.Valid))
				clip.SetInstructions(Enumerable.Empty<string>());
			else
				clip.SetInstructions(instructions.Select(s => s.Text));

			_clipModified = true;
		}

		private List<AnimInstructionView> ParseClipInstructions(string editorText)
		{
			List<AnimInstructionView> result = new List<AnimInstructionView>();

			if (string.IsNullOrEmpty(editorText))
				return result;

			var lines = editorText.Split('\n');

			foreach (var line in lines) {
				AnimInstructionView view = new AnimInstructionView()
				{
					Text = line
				};
				result.Add(view);

				if (line.Trim().EndsWith(":")) {
					view.IsLabel = true;
					view.Valid = true;
					continue;
				}

				var split = line.Split(' ');
				string instText = split.FirstOrDefault();

				if (instText == null)
					continue;

				if (!AnimClipInstructionDatabase.Instructions.TryGetValue(instText, out view.Instruction))
					continue;

				if (view.Instruction.Parameters.Count != split.Length - 1)
					continue;

				view.Parameters = new object[view.Instruction.Parameters.Count];
				view.Valid = true;

				for (int j = 0; j < view.Instruction.Parameters.Count; ++j) {
					var paramObj = view.Instruction.Parameters[j].Parse(split[j + 1], out view.Valid);

					if (!view.Valid)
						break;
					else
						view.Parameters[j] = paramObj;
				}
			}

			return result;
		}

	}
}
