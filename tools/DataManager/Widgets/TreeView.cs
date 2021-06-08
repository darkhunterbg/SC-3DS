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
		private TreeItem root = new TreeItem();
		public readonly string Id;
		public ITreeViewItem Selected { get; set; }


		public TreeView(string id)
		{
			Id = id;
		}


		private void NewItem(TreeItem parent, IEnumerable<string> path, ITreeViewItem item = null)
		{
			string itemName = path.FirstOrDefault();
			bool isDir = path.Count() > 1;
			TreeItem next = parent.Children.FirstOrDefault(t => t.Name == itemName && (isDir == t.IsDir));

			if (next == null)
			{
				next = new TreeItem()
				{
					Name = itemName,
				};
				parent.Children.Add(next);
			}

			if (!isDir)
			{
				next.Item = item;
			}
			else
			{
				NewItem(next, path.Skip(1), item);
			}


		}

		private void RegenerateItems()
		{
			root = new TreeItem();

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

			foreach (var child in root.ChildrenView)
			{
				DrawTreeItem(child);
			}

		}
		private void DrawTreeItem(TreeItem node)
		{
			ImGui.PushID(node.Id);

			ImGuiTreeNodeFlags flags = ImGuiTreeNodeFlags.SpanAvailWidth;

			if (node.Opened)
				flags |= ImGuiTreeNodeFlags.DefaultOpen;

			if (!node.IsDir || node.Children.Count == 0)
			{
				bool selected = Selected == node.Item;
				ImGui.Indent(ImGui.GetTreeNodeToLabelSpacing());
				if (ImGui.Selectable(node.Name, selected))
				{
					if (selected)
						Selected = null;
					else
						Selected = node.Item;
				}
				ImGui.Unindent(ImGui.GetTreeNodeToLabelSpacing());
			}
			else
			{
				node.Opened = ImGui.TreeNodeEx(node.Name, flags);
			}





			if (node.Opened)
			{
				foreach (var child in node.ChildrenView)
				{
					DrawTreeItem(child);
				}
			}
			ImGui.TreePop();

			ImGui.PopID();

		}
	}
}
