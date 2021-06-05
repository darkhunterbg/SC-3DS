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
				editorDrawers.Add(EditorPropertyDrawer.GetPropertyDrawer(editorProperties[i], editorAttributes[i]));
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
		}

		private void TableRow(TItem item)
		{
			int itemId = (int)uniqueKeyField.GetValue(item);

			for (int i = 0; i < editorAttributes.Count; ++i)
			{
				ImGui.TableNextColumn();

				var attr = editorAttributes[i];
				var prop = editorProperties[i];

				ImGui.PushID(itemId);

				if (editorDrawers[i](prop, attr, item))
					changed = true;

				ImGui.PopID();

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


	}
}
