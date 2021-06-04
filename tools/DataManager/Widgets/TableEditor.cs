using DataManager.Assets;
using DataManager.Panels;
using ImGuiNET;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Numerics;
using System.Reflection;
using System.Text;
using System.Threading.Tasks;

namespace DataManager.Widgets
{
	public class TableEditor<TItem> where TItem : class
	{
		delegate void EditorDrawAction(PropertyInfo prop, EditorAttribute attr, TItem item, int itemId, int propId);

		public delegate TItem NewItemAction(TItem copy);

		public TItem SelectedItem { get; set; }
		public TItem HoverItem { get; private set; }

		public List<TItem> DataSource { get; set; }

		public IEnumerable<TItem> View { get; private set; }

		private List<PropertyInfo> editorProperties;
		private List<EditorAttribute> editorAttributes;
		private List<EditorDrawAction> editorDrawers;

		private FieldInfo uniqueKeyField;
		private readonly string id;

		private bool changed = false;

		public bool HasChanges => changed;

		private TItem modalEditItem = null;
		private object modalSelectedValue = null;
		private int modalEditPropertyId = -1;
		private string modalFilter = string.Empty;

		private object tooltipHover = null;

		public NewItemAction OnNewItem;

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

			uniqueKeyField = type.GetFields().FirstOrDefault(t => t.GetCustomAttribute<UniqueKeyAttribute>() != null);
		}

		public void Draw()
		{
			DrawControlHeader();

			if (!ImGui.BeginTable(id, editorProperties.Count + 1, ImGuiTableFlags.BordersInnerH
	| ImGuiTableFlags.ScrollY | ImGuiTableFlags.RowBg | ImGuiTableFlags.Resizable))
				return;
			HoverItem = null;
			changed = false;
			tooltipHover = null;
			ImGui.TableSetupScrollFreeze(0, 1);

			foreach (var p in editorProperties)
			{
				var editorAttr = p.GetCustomAttribute<EditorAttribute>();

				ImGui.TableSetupColumn(editorAttr.Name ?? p.Name);
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

			ImGui.TableNextColumn();
			DrawControlCell(item, itemId);

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

		private void DrawControlCell(TItem item, int itemId)
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
			}
			else
			{
				if (attr is CustomEnumEditorAttribute)
					return CustomEnumEditor;

				if (attr is ImageEditorAttribute)
					return LogicalImageEditor;
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
				modalEditPropertyId = propId;
			}
		}

		private void DrawChangeImageModal()
		{
			bool opened = modalEditItem != null;

			if (!opened)
				return;

			var tmp = modalSelectedValue as LogicalImageAsset;

			ImGui.OpenPopup("table.modal.edit.image");

			if (!ImGui.BeginPopupModal("table.modal.edit.image", ref opened, ImGuiWindowFlags.NoTitleBar | ImGuiWindowFlags.AlwaysAutoResize))
				return;

			ImGui.Text(tmp?.SpriteSheetName ?? string.Empty);

			ImGui.InputText("##table.modal.edit.image.filter", ref modalFilter, 255);

			var query = ImageEditor.GetImages(modalFilter);

			query = query.OrderBy(asset => asset.SpriteSheetName);

			int i = 0;

			ImGui.BeginChild("table.modal.edit.image.items", new Vector2(600, 600));

			foreach (var asset in query)
			{
				bool selected = tmp == asset;

				if (ImGui.Selectable($"##table.modal.edit.image.select.{i++}", selected))
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

			ImGui.EndChild();

			if (ImGui.Button("Cancel##table.modal.edit.image.cancel"))
			{
				modalEditItem = null;
				modalSelectedValue = null;
				modalEditPropertyId = -1;
				modalFilter = string.Empty;

			}

			ImGui.SameLine();

			if (tmp != null)
			{
				if (ImGui.Button("Ok##table.modal.edit.image.ok"))
				{
					var prop = editorProperties[modalEditPropertyId];
					prop.SetValue(modalEditItem, tmp);
					tmp = null;
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
