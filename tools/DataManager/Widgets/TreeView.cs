using ImGuiNET;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace DataManager.Widgets
{
	public class TreeView
	{
		class TreeItem
		{
			public string Name = string.Empty;

			public ITreeViewItem Item;

			private static int _id = 0;

			public readonly int Id = ++_id;

			public bool Opened = false;

			public readonly List<TreeItem> Children = new List<TreeItem>();

			public bool IsDir => Item == null;

			public IEnumerable<TreeItem> ChildrenView =>
				Children.OrderByDescending(t => t.IsDir)
				.ThenBy(t => t.Name);

			public override string ToString()
			{
				return $"[{Id}] {Name}";
			}

			public TreeItem Parent { get; set; }

			public TreeItem(TreeItem parent)
			{
				Parent = parent;
			}

			public string GetPath(TreeItem root)
			{
				List<string> result = new List<string>() { Name };
				var p = Parent;
				while (p != null && p != root)
				{
					result.Insert(0, p.Name);
					p = p.Parent;
				}

				return string.Join("\\", result);
			}
		}

		private IEnumerable<ITreeViewItem> dataSource;

		public IEnumerable<ITreeViewItem> DataSource
		{
			get
			{
				return DataSource;
			}
			set
			{
				dataSource = value;
				RegenerateItems();
			}
		}

		public Func<ITreeViewItem, ITreeViewItem> NewItemAction;
		public Action<ITreeViewItem> DeleteItemAction;

		private TreeItem root = new TreeItem(null) { Name = "Items", Opened = true };
		public readonly string Id;
		public ITreeViewItem Selected { get; set; }

		public TreeView(string id)
		{
			Id = id;
		}

		private void NewItem(TreeItem parent, IEnumerable<string> path, ITreeViewItem item = null)
		{
			string itemName = path.FirstOrDefault();
			bool isLastItem = path.Count() == 1;

			if (isLastItem)
			{
				parent.Children.Add(new TreeItem(parent)
				{
					Name = itemName,
					Item = item,
				});
			}
			else
			{
				TreeItem next = parent.Children.FirstOrDefault(t => t.Name == itemName);
				if (next == null)
				{
					parent.Children.Add(next = new TreeItem(parent)
					{
						Name = itemName,
					});
				}
				NewItem(next, path.Skip(1), item);
			}


		}
		private void GetItemsInChildren(TreeItem node, List<ITreeViewItem> outItems)
		{
			if (node.Item != null)
				outItems.Add(node.Item);

			foreach (var child in node.Children)
				GetItemsInChildren(child, outItems);
		}
		private void RegenerateItems()
		{
			root.Children.Clear();

			if (dataSource == null)
				return;

			foreach (var item in dataSource
				.OrderBy(t => t.Path.Count(c => c == '\\'))
				.ThenBy(t => t.Path))
			{

				var s = item.Path.Split('\\');

				NewItem(root, s, item);
			}
		}

		public void Draw()
		{
			DrawTreeItem(root);
		}
		private void DrawTreeItem(TreeItem node)
		{
			ImGui.PushID(node.Id);

			ImGuiTreeNodeFlags flags = ImGuiTreeNodeFlags.SpanAvailWidth;

			if (node.Opened)
				flags |= ImGuiTreeNodeFlags.DefaultOpen;


			if (!node.IsDir || node.Children.Count == 0)
			{
				bool selected = Selected != null && (Selected == node.Item);
				ImGui.Indent(ImGui.GetTreeNodeToLabelSpacing());
				if (ImGui.Selectable(node.Name, selected))
				{
					if (selected)
						Selected = null;
					else
						Selected = node.Item;
				}
				ImGui.Unindent(ImGui.GetTreeNodeToLabelSpacing());

				ItemContextMenu(node);
			}
			else
			{
				node.Opened = ImGui.TreeNodeEx(node.Name, flags);

				ItemContextMenu(node);

				if (node.Opened)
				{
					foreach (var child in node.ChildrenView)
					{
						DrawTreeItem(child);
					}
					ImGui.TreePop();
				}

			}
			ImGui.PopID();
		}

		private void ItemContextMenu(TreeItem node)
		{
			if (ImGui.BeginPopupContextItem())
			{
				if (node.IsDir)
				{
					if (ImGui.Selectable("New Folder"))
					{
						NewItem(node, new string[] { "New Folder" });
						node.Opened = true;
						ImGui.CloseCurrentPopup();
					}
					if (NewItemAction != null)
					{
						if (ImGui.Selectable("New Item"))
						{
							var item = NewItemAction(null);
							if (node != root)
								item.Path = node.GetPath(root) + "\\New Item";
							else
								item.Path = "New Item";
							NewItem(node, new string[] { "New Item" }, item);
							node.Opened = true;
							ImGui.CloseCurrentPopup();
						}
					}

					if (node != root && DeleteItemAction != null)
					{
						if (ImGui.Selectable("Delete"))
						{
							List<ITreeViewItem> items = new List<ITreeViewItem>();
							GetItemsInChildren(node, items);

							node.Parent.Children.Remove(node);
							foreach (var item in items)
								DeleteItemAction(item);
							ImGui.CloseCurrentPopup();
						}
					}
				}
				else
				{
					if (NewItemAction != null)
					{
						if (ImGui.Selectable("Duplicate"))
						{
							var item = NewItemAction(node.Item);
							string name = $"Copy of {node.Name}";
							if (node != root)
								item.Path = node.GetPath(root) + $"\\{name}";
							else
								item.Path = name;
							NewItem(node.Parent, new string[] { name }, item);
							ImGui.CloseCurrentPopup();
						}
					}
					if (DeleteItemAction != null)
					{
						if (ImGui.Selectable("Delete"))
						{
							node.Parent.Children.Remove(node);
							DeleteItemAction(node.Item);
							ImGui.CloseCurrentPopup();
						}
					}
				}
				ImGui.EndPopup();
			}
		}
	}
}