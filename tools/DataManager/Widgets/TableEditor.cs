using DataManager.Assets;
using DataManager.Panels;
using ImGuiNET;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Numerics;
using System.Reflection;
using System.Text;
using System.Text.RegularExpressions;
using System.Threading.Tasks;

namespace DataManager.Widgets
{

	public class TableEditor<TItem> where TItem : class
	{
		enum ModalSelectionType
		{
			ImageList,
			Icon
		}

		delegate void EditorDrawAction(PropertyInfo prop, EditorAttribute attr, TItem item, int itemId, int propId);

		public delegate TItem NewItemAction(TItem copy);

		public TItem SelectedItem { get; set; }
		public TItem HoverItem { get; private set; }

		public List<TItem> DataSource { get; set; }

		public IEnumerable<TItem> View { get; private set; }

		private List<PropertyInfo> editorProperties;
		private List<EditorAttribute> editorAttributes;
		private List<EditorDrawAction> editorDrawers;
		private List<string> columns = new List<string>();

		private FieldInfo uniqueKeyField;
		private readonly string id;

		private bool changed = false;

		public bool HasChanges => changed;

		private TItem modalEditItem = null;
		private object modalSelectedValue = null;
		private int modalEditPropertyId = -1;
		private string modalFilter = string.Empty;
		private ModalSelectionType modalSelectionType;

		private object tooltipHover = null;

		public NewItemAction OnNewItem;

		public bool DrawControlCell = true;

		public TableEditor(string id)
		{
			this.id = id;
			var type = typeof(TItem);
			editorProperties = type.GetProperties().Where(t => t.GetCustomAttribute<EditorAttribute>() != null).ToList();
			editorAttributes = editorProperties.Select(t => t.GetCustomAttribute<EditorAttribute>()).ToList();

			editorDrawers = new List<EditorDrawAction>();
			for (int i = 0; i < editorProperties.Count; ++i)
			{
				editorDrawers.Add(GetPropertyDrawer(editorProperties[i], editorAttributes[i]));
			}

			foreach (var p in editorProperties)
			{
				var editorAttr = p.GetCustomAttribute<EditorAttribute>();
				string name = editorAttr.Name ?? p.Name;
				name = Regex.Replace(name, "(\\B[A-Z])", " $1");
				columns.Add(name);
			}

			uniqueKeyField = type.GetFields().FirstOrDefault(t => t.GetCustomAttribute<UniqueKeyAttribute>() != null);
		}

		public void Draw()
		{
			HoverItem = null;
			changed = false;
			tooltipHover = null;

			DrawControlHeader();

			if (!ImGui.BeginTable(id, editorProperties.Count + 1, ImGuiTableFlags.BordersInnerH
	| ImGuiTableFlags.ScrollY | ImGuiTableFlags.RowBg | ImGuiTableFlags.Resizable ))
				return;

			ImGui.TableSetupScrollFreeze(0, 1);


			foreach (var c in columns)
			{
				ImGui.TableSetupColumn(c);
			}

			ImGui.TableSetupColumn(string.Empty, ImGuiTableColumnFlags.WidthFixed, 200);

			ImGui.TableHeadersRow();

			foreach (var item in DataSource.ToList())
			{
				ImGui.TableNextRow();

				TableRow(item);

			}


			ImGui.EndTable();

			DrawChangeImageModal();
			DrawHoverTooltip();
		}

		private void TableRow(TItem item)
		{
			int itemId = (int)uniqueKeyField.GetValue(item);

			for (int i = 0; i < editorAttributes.Count; ++i)
			{
				ImGui.TableNextColumn();

				var attr = editorAttributes[i];
				var prop = editorProperties[i];

				editorDrawers[i](prop, attr, item, itemId, i);

			}

			if (DrawControlCell)
			{
				ImGui.TableNextColumn();
				ControlCell(item, itemId);
			}

			ImGui.SameLine();
			bool selected = SelectedItem == item;

			if (ImGui.Selectable($"##{id}.items.{itemId}.selected", ref selected, ImGuiSelectableFlags.SpanAllColumns))
			{
				if (selected)
					SelectedItem = item;
				else
					SelectedItem = null;
			}


			if (ImGui.IsItemHovered())
			{
				HoverItem = item;
			}
		}

		private void ControlCell(TItem item, int itemId)
		{
			if (ImGui.Button($"Duplicate##{id}.items.{itemId}.control.copy"))
			{
				int index = DataSource.IndexOf(item);
				var newItem = OnNewItem(item);
				DataSource.Insert(index + 1, newItem);
				changed = true;
			}
			ImGui.SameLine();
			if (ImGui.Button($"Delete##{id}.items.{itemId}.control.del"))
			{
				DataSource.Remove(item);
				changed = true;
			}
		}

		private void DrawControlHeader()
		{
			//ImGui.InputText("##{}.filter", ref filter, 255);

			if (ImGui.Button($"New Item##{id}.new"))
			{
				if (OnNewItem != null)
				{
					DataSource.Add(OnNewItem(null));
					changed = true;
				}
			}

			if (SelectedItem != null)
			{
				ImGui.SameLine();

				if (ImGui.Button($"Duplicate Selected##{id}.copy"))
				{
					if (SelectedItem != null && OnNewItem != null)
					{
						int index = DataSource.IndexOf(SelectedItem);
						var newItem = OnNewItem(SelectedItem);
						DataSource.Insert(index + 1, newItem);
						SelectedItem = newItem;
						changed = true;
					}
				}

				ImGui.SameLine();

				if (ImGui.Button($"Delete Selected##{id}.del"))
				{
					if (SelectedItem != null)
					{
						DataSource.Remove(SelectedItem);
						changed = true;
						SelectedItem = null;
					}

				}
			}
		}

		string text;
		int number;

		private EditorDrawAction GetPropertyDrawer(PropertyInfo prop, EditorAttribute attr)
		{
			if (!prop.CanWrite || attr.ReadOnly)
				return ReadOnlyEditor;

			if (attr is DefaultEditorAttribute)
			{
				var type = prop.PropertyType;

				if (type == typeof(string))
					return StringEditor;

				if (type == typeof(int))
					return IntEditor;

				if (type.IsEnum)
					return EnumEditor;
			}
			else
			{
				if (attr is CustomEnumEditorAttribute)
					return CustomEnumEditor;

				if (attr is ImageEditorAttribute)
					return LogicalImageEditor;

				if (attr is IconEditorAttribute)
					return IconEditor;

				if (attr is FrameTimeEditorAttribute)
					return this.FrameTimeEditor;
				
			}


			return ReadOnlyEditor;
		}

		private void ReadOnlyEditor(PropertyInfo prop, EditorAttribute attr, TItem item, int itemId, int propId)
		{
			ImGui.Text(prop.GetValue(item)?.ToString() ?? string.Empty);
		}
		private void StringEditor(PropertyInfo prop, EditorAttribute attr, TItem item, int itemId, int propId)
		{
			text = prop.GetValue(item)?.ToString() ?? string.Empty;

			AppGui.StrechNextItem();
			if (ImGui.InputText($"##{id}.items.{itemId}.{propId}", ref text, 256))
			{
				prop.SetValue(item, text);
				changed = true;
			}

		}
		private void IntEditor(PropertyInfo prop, EditorAttribute attr, TItem item, int itemId, int propId)
		{
			number = (int)prop.GetValue(item);

			AppGui.StrechNextItem();
			if (ImGui.InputInt($"##{id}.items.{itemId}.{propId}", ref number))
			{
				prop.SetValue(item, number);
				changed = true;
			}

		}

		private void EnumEditor(PropertyInfo prop, EditorAttribute attr, TItem item, int itemId, int propId)
		{
			number = (int)prop.GetValue(item);
			AppGui.StrechNextItem();
			var enumType = prop.PropertyType;
			var enumValues = EnumCacheValues.GetValues(enumType);
			if (ImGui.Combo($"##{id}.items.{itemId}.{propId}", ref number, enumValues, enumValues.Length))
			{
				prop.SetValue(item, number);
				changed = true;
			}
		}

		private void CustomEnumEditor(PropertyInfo prop, EditorAttribute attr, TItem item, int itemId, int propId)
		{
			var customAttr = attr as CustomEnumEditorAttribute;

			var enumValues = CustomEnumValues.CustomEnums[(int)customAttr.EnumType];

			number = (int)prop.GetValue(item);
			AppGui.StrechNextItem();
			if (ImGui.Combo($"##{id}.items.{itemId}.{propId}", ref number, enumValues, enumValues.Length))
			{
				prop.SetValue(item, number);
				changed = true;
			}
		}
		private void LogicalImageEditor(PropertyInfo prop, EditorAttribute attr, TItem item, int itemId, int propId)
		{
			var image = prop.GetValue(item) as LogicalImageAsset;

			ImGui.Text(image.SpriteSheetName ?? string.Empty);

			if (ImGui.IsItemHovered())
			{
				tooltipHover = image;
			}

			ImGui.SameLine();

			if (ImGui.Button($"Change##{id}.items.{itemId}.{propId}"))
			{
				modalEditItem = item;
				modalSelectedValue = prop.GetValue(modalEditItem);
				modalSelectionType = ModalSelectionType.ImageList;
				modalEditPropertyId = propId;
			}
		}

		private void IconEditor(PropertyInfo prop, EditorAttribute attr, TItem item, int itemId, int propId)
		{
			var icon = prop.GetValue(item) as SpriteFrameAsset;

			if (ImGui.ImageButton(icon.Image.GuiImage, icon.SpriteSheet.FrameSize, Vector2.Zero,
				Vector2.One, 0,
				Vector4.Zero,
				Microsoft.Xna.Framework.Color.Yellow.ToVec4()))
			{
				modalEditItem = item;
				modalSelectedValue = icon;
				modalSelectionType = ModalSelectionType.Icon;
				modalEditPropertyId = propId;
			}

		}

		private void FrameTimeEditor(PropertyInfo prop, EditorAttribute attr, TItem item, int itemId, int propId)
		{
			number = (int)prop.GetValue(item);

			ImGui.SetNextItemWidth(100);
			if (ImGui.InputInt($"##{id}.items.{itemId}.{propId}", ref number,0))
			{
				prop.SetValue(item, number);
				changed = true;
			}
			ImGui.SameLine();
			ImGui.Text($"{(number / 15.0f).ToString("F1")} sec");

		}

		private string ChangeModalSelectedText()
		{
			if (modalSelectionType == ModalSelectionType.ImageList)
				return (SelectedItem as LogicalImageAsset)?.SpriteSheet?.SheetName ?? string.Empty;

			return null;
		}
		private bool ModalCanSearch()
		{
			return modalSelectionType == ModalSelectionType.ImageList;
		}
		private void ModalDrawSelection(object item, string filter)
		{
			int i = 0;
			if (modalSelectionType == ModalSelectionType.ImageList)
			{
				var tmp = item as LogicalImageAsset;

				var query = ImageEditor.GetImages(modalFilter);
				query = query.OrderBy(asset => asset.SpriteSheetName);

				foreach (var asset in query)
				{
					bool selected = tmp == asset;

					if (ImGui.Selectable($"##table.modal.edit.select.{i++}", selected))
					{
						if (selected)
							modalSelectedValue = null;
						else
							modalSelectedValue = asset;

					}
					ImGui.SameLine();
					ImGui.Text(asset.SpriteSheetName);
					if (ImGui.IsItemHovered())
					{
						ImGui.BeginTooltip();

						AppGui.DrawSpriteSheetInfo(asset.SpriteSheet);

						ImGui.EndTooltip();
					}
				}
				return;
			}
			if (modalSelectionType == ModalSelectionType.Icon)
			{
				var image = item as SpriteFrameAsset;
				var query = AppGame.AssetManager.Icons;

				foreach (var asset in query)
				{
					i++;
					bool selected = image == asset;

					Vector4 color = selected ? Vector4.One : Vector4.Zero;

					if (ImGui.ImageButton(asset.Image.GuiImage, new Vector2(64, 64), Vector2.Zero,
				Vector2.One, 2,
				color,
				Microsoft.Xna.Framework.Color.Yellow.ToVec4()))
					{
						if (selected)
							modalSelectedValue = null;
						else
							modalSelectedValue = asset;
					}


					if (i % 10 != 0)
						ImGui.SameLine();
				}
				return;
			}
		}

		private void DrawChangeImageModal()
		{
			bool opened = modalEditItem != null;

			if (!opened)
				return;
			ImGui.OpenPopup("table.modal.edit");

			if (!ImGui.BeginPopupModal("table.modal.edit", ref opened, ImGuiWindowFlags.NoTitleBar | ImGuiWindowFlags.AlwaysAutoResize))
				return;

			string text = ChangeModalSelectedText();
			if (text != null)
				ImGui.Text(text);

			bool canSearch = ModalCanSearch();
			if (canSearch)
				ImGui.InputText("##table.modal.edit.filter", ref modalFilter, 255);


			ImGui.BeginChild("table.modal.edit.items", new Vector2(800, 800));

			ModalDrawSelection(modalSelectedValue, modalFilter);

			ImGui.EndChild();

			if (ImGui.Button("Cancel##table.modal.cancel"))
			{
				modalEditItem = null;
				modalSelectedValue = null;
				modalEditPropertyId = -1;
				modalFilter = string.Empty;

			}

			ImGui.SameLine();

			if (modalSelectedValue != null)
			{
				if (ImGui.Button("Ok##table.modal.image.ok"))
				{
					var prop = editorProperties[modalEditPropertyId];
					prop.SetValue(modalEditItem, modalSelectedValue);

					modalEditItem = null;
					modalSelectedValue = null;
					modalEditPropertyId = -1;
					changed = true;
					modalFilter = string.Empty;
				}
			}
			ImGui.EndPopup();
		}

		private void DrawHoverTooltip()
		{
			if (tooltipHover == null)
			{
				return;
			}

			ImGui.BeginTooltip();

			if (tooltipHover is LogicalImageAsset)
			{
				AppGui.DrawSpriteSheetInfo(((LogicalImageAsset)tooltipHover).SpriteSheet);
			}

			ImGui.EndTooltip();
		}

	}
}
