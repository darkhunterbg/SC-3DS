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
	public class TablePropertyEditor
	{
		public bool Changed { get; private set; }

		private object editItem = null;

		private readonly string id;

		public object EditingItem
		{
			get { return editItem; }
			set
			{
				if (editItem == value)
					return;
				editItem = value;
				RebindItem();
			}
		}

		private List<PropertyInfo> editorProperties = new List<PropertyInfo>();
		private List<EditorAttribute> editorAttributes = new List<EditorAttribute>();
		private List<EditorDrawAction> editorDrawers = new List<EditorDrawAction>();
		private List<string> propertyNames = new List<string>();

		public TablePropertyEditor(string id)
		{
			this.id = id;
		}

		private void RebindItem()
		{
			editorProperties.Clear();
			editorAttributes.Clear();
			editorDrawers.Clear();
			propertyNames.Clear();

			if (editItem == null)
				return;

			var type = editItem.GetType();
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
				propertyNames.Add(name);
			}
		}

		public void Draw(Vector2 size = new Vector2())
		{
			Changed = false;

			if (!ImGui.BeginTable(id, 2,  ImGuiTableFlags.ScrollY | ImGuiTableFlags.RowBg |ImGuiTableFlags.Resizable, size))
				return;

			ImGui.TableSetupScrollFreeze(0, 1);

			ImGui.TableSetupColumn("Name", ImGuiTableColumnFlags.WidthFixed, 400);
			ImGui.TableSetupColumn("Value");

			ImGui.TableHeadersRow();

			for (int i = 0; i < editorProperties.Count; ++i)
			{
				ImGui.TableNextRow();

				ImGui.PushID(i);

				ImGui.TableNextColumn();

				ImGui.Text(propertyNames[i]);

				ImGui.TableNextColumn();

				var attr = editorAttributes[i];
				var prop = editorProperties[i];

				if (editorDrawers[i](prop, attr, editItem))
					Changed = true;

				ImGui.PopID();
			}

			ImGui.EndTable();
		}
	}
}
