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
		class Section
		{
			public string Name = string.Empty;

			public List<PropertyInfo> editorProperties = new List<PropertyInfo>();
			public List<EditorAttribute> editorAttributes = new List<EditorAttribute>();
			public List<EditorDrawAction> editorDrawers = new List<EditorDrawAction>();
			public List<string> propertyNames = new List<string>();
			public List<object> targets = new List<object>();

			public override string ToString()
			{
				return Name;
			}
		}

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

		private List<Section> sections = new List<Section>();

		public TablePropertyEditor(string id)
		{
			this.id = id;
		}

		private void RebindItem()
		{
			sections.Clear();
	
			if (editItem == null)
				return;

			sections.Add(new Section() { Name = "Properties" });

			CollectFields(editItem, editItem.GetType());
			sections.RemoveAll(s => s.propertyNames.Count == 0);
			sections = sections.OrderBy(s => s.Name).ToList();
		}

		private void CollectFields(object target, Type type)
		{
			var editorProperties = type.GetProperties().Where(t => t.GetCustomAttribute<EditorAttribute>() != null).ToList();

			Section current = sections.Last();
			foreach (var p in editorProperties)
			{
				var sectAttr = p.GetCustomAttribute<SectionAttribute>();
				if (sectAttr != null)
				{
					current = new Section()
					{
						Name = sectAttr.Name
					};
					sections.Add(current);
				}

				var editorAttr = p.GetCustomAttribute<EditorAttribute>();

				if (editorAttr is ArrayEditorAttribute arrayAttr)
				{
					var arrayElemType = p.PropertyType.GetElementType();
					var array = p.GetValue(target) as Array;

					string prevSectionName = current.Name;

					for (int i = 0; i < arrayAttr.ArraySize; ++i)
					{
						if (array.GetValue(i) == null)
							array.SetValue(Activator.CreateInstance(arrayElemType), i);

						string sectionName = Regex.Replace((editorAttr.Name ?? p.Name), "(\\B[A-Z])", " $1") + $" {i + 1}";

						current = new Section()
						{
							Name = string.IsNullOrEmpty(prevSectionName) ? sectionName :  $"{prevSectionName}\\{sectionName}"
						};
						sections.Add(current);

						CollectFields(array.GetValue(i), arrayElemType);
					}
				}
				else
				{
					current.editorAttributes.Add(editorAttr);
					current.editorProperties.Add(p);
					current.editorDrawers.Add(EditorPropertyDrawer.GetPropertyDrawer(p, editorAttr));
					current.targets.Add(target);

					string name = editorAttr.Name ?? p.Name;
					name = Regex.Replace(name, "(\\B[A-Z])", " $1");
					current.propertyNames.Add(name);
				}
			}
		}


		static int rowIndex = 0;
		public void Draw(Vector2 size = new Vector2())
		{
			Changed = false;

			rowIndex = 0;
			foreach (var section in sections)
			{
				//ImGui.PushID(++i);
				DrawSection(section);
				//ImGui.PopID();
			}


		}

		private void DrawSection(Section section)
		{
			if (!ImGui.BeginTable(id, 2,  ImGuiTableFlags.BordersOuter ))
				return;

			ImGui.TableSetupScrollFreeze(0, 1);

			ImGui.TableSetupColumn(section.Name, ImGuiTableColumnFlags.WidthFixed, 200);
			ImGui.TableSetupColumn(string.Empty);

			ImGui.TableHeadersRow();


			for (int i = 0; i < section.editorProperties.Count; ++i)
			{
				++rowIndex;
				ImGui.TableNextRow();

				ImGui.PushID(rowIndex);

				ImGui.TableNextColumn();

				ImGui.Text(section.propertyNames[i]);

				ImGui.TableNextColumn();

				var attr = section.editorAttributes[i];
				var prop = section.editorProperties[i];
				var target = section.targets[i];

				if (section.editorDrawers[i](prop, attr, target))
					Changed = true;


				ImGui.PopID();
			}

			ImGui.EndTable();
		}
	}
}
