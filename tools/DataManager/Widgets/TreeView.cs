using ImGuiNET;
using Microsoft.Xna.Framework.Input;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Numerics;
using System.Text;
using System.Threading.Tasks;
using Color = Microsoft.Xna.Framework.Color;

namespace DataManager.Widgets
{
	public class TreeView
	{
		enum ContextOperation
		{
			Copy,
			Cut
		}

		class TreeItem
		{
			public string Name = string.Empty;

			public ITreeViewItem Item;

			private static int _id = 0;

			public readonly int Id = ++_id;

			public bool Expanded = false;

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

			public bool UpdateItemPath(TreeItem root)
			{
				if (Item != null)
				{
					Item.Path = GetPath(root);
				}

				bool anyUpdate = Item != null;

				foreach (var child in Children)
				{
					if (child.UpdateItemPath(root))
						anyUpdate = true;
				}

				return anyUpdate;
			}
		
			public int GetItemCount()
			{
				int i = Item == null ? 0 : 1;

				foreach (var c in Children)
					i += c.GetItemCount();

				return i;
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

		private TreeItem root = new TreeItem(null) { Name = "Items", Expanded = true };
		public readonly string Id;

		private bool focused = false;

		private TreeItem selectedNode;
		public ITreeViewItem Selected => selectedNode?.Item;
		public string ItemName = "Item";
		public string RootName
		{
			get { return root.Name; }
			set { root.Name = value; }
		}

		private TreeItem renameNode;
		private string renameText = string.Empty;
		private bool renameFocusRequest = false;

		public bool ItemModified { get; private set; }

		public TreeView(string id)
		{
			Id = id;
		}

		private TreeItem NewItem(TreeItem parent, IEnumerable<string> path, ITreeViewItem item = null)
		{
			string itemName = path.FirstOrDefault();
			bool isLastItem = path.Count() == 1;

			if (isLastItem)
			{
				TreeItem newChild = new TreeItem(parent)
				{
					Name = itemName,
					Item = item,
					Expanded = item == null
				};
				parent.Children.Add(newChild);
				return newChild;
			}
			else
			{
				TreeItem next = parent.Children.FirstOrDefault(t => t.Name == itemName && t.IsDir);
				if (next == null)
				{
					parent.Children.Add(next = new TreeItem(parent)
					{
						Name = itemName,
						Expanded = true
					});
				}
				return NewItem(next, path.Skip(1), item);
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
		private bool NodeExists(TreeItem node, TreeItem root)
		{
			if (root == node)
				return true;

			foreach (var child in root.Children)
			{
				if (NodeExists(node, child))
					return true;
			}

			return false;
		}

		private void GetVisibleNodes(TreeItem node, List<TreeItem> result)
		{
			result.Add(node);
			if (node.Expanded)
				foreach (var c in node.Children)
					GetVisibleNodes(c, result);
		}
		private IEnumerable<TreeItem> GetDirectories(TreeItem node)
		{
			if (node.IsDir)
			{
				yield return node;

				foreach (var c in node.Children)
					foreach (var n in GetDirectories(c))
						yield return n;
			}
		}
		private TreeItem NextNode(TreeItem node)
		{
			List<TreeItem> nodes = new List<TreeItem>();
			GetVisibleNodes(root, nodes);
			int index = nodes.IndexOf(node);
			++index;
			if (index >= nodes.Count)
				return node;

			return nodes[index];
		}
		private TreeItem PrevNode(TreeItem node)
		{
			List<TreeItem> nodes = new List<TreeItem>();
			GetVisibleNodes(root, nodes);
			int index = nodes.IndexOf(node);
			--index;
			if (index < 0)
				return node;

			return nodes[index];
		}

		public void Draw()
		{
			focused = false;
			ItemModified = false;
			DrawTreeItem(root);

			if (selectedNode != null && focused)
			{
				ProcessInput();
			}
		}

		private TreeItem contextNode = null;
		private ContextOperation contextOp;
		private void ProcessInput()
		{
			if (renameNode != null)
				return;

			if (AppGame.Gui.IsButtonPressed(Keys.F2))
				RenameNode(selectedNode);
			if (AppGame.Gui.IsButtonPressed(Keys.Delete))
				DeleteNode(selectedNode);

			if (AppGame.Gui.IsButtonPressed(Keys.D) && ImGui.GetIO().KeyCtrl)
			{
				if (Selected != null)
					DuplicateNode(selectedNode);
			}

			if (AppGame.Gui.IsButtonPressed(Keys.Right))
			{
				if (!selectedNode.Expanded && selectedNode.IsDir)
					selectedNode.Expanded = true;
				else if (selectedNode.Children.Count > 0)
					selectedNode = selectedNode.Children[0];

			}

			if (AppGame.Gui.IsButtonPressed(Keys.Left))
			{
				if (selectedNode.Expanded && selectedNode.IsDir)
					selectedNode.Expanded = false;
				else if (selectedNode.Parent != null)
					selectedNode = selectedNode.Parent;

			}

			if (AppGame.Gui.IsButtonPressed(Keys.Down))
			{
				selectedNode = NextNode(selectedNode);
			}

			if (AppGame.Gui.IsButtonPressed(Keys.Up))
			{
				selectedNode = PrevNode(selectedNode);
			}

			if (AppGame.Gui.IsButtonPressed(Keys.C) && ImGui.GetIO().KeyCtrl)
			{
				contextNode = selectedNode;
				contextOp = ContextOperation.Copy;
			}
			if (AppGame.Gui.IsButtonPressed(Keys.X) && ImGui.GetIO().KeyCtrl)
			{
				contextNode = selectedNode;
				contextOp = ContextOperation.Cut;
			}
			if (AppGame.Gui.IsButtonPressed(Keys.V) && ImGui.GetIO().KeyCtrl && contextNode != null)
			{
				if (contextOp == ContextOperation.Copy)
					DuplicateNode(contextNode, selectedNode, true);
				else
				{
					if (contextOp == ContextOperation.Cut)
					{
						MoveNode(contextNode, selectedNode);
						contextNode = null;
					}
				}
			}
		}

		private void DrawTreeItem(TreeItem node)
		{
			ImGui.PushID(node.Id);

			bool selected = selectedNode == node;

			if (!node.IsDir)
			{
				//ImGui.Indent(ImGui.GetTreeNodeToLabelSpacing());

				if (node.Item.Preview == null)
					ImGui.Bullet();
				else
					ImGui.Image(node.Item.Preview.GuiImage, new Vector2(24, 24), Vector2.Zero, Vector2.One,
						Vector4.One, Vector4.One);

				ImGui.SameLine();

				if (renameNode == node)
				{
					DrawRenameNode();
				}
				else
				{
					if (contextNode == node && contextOp == ContextOperation.Cut)
						ImGui.PushStyleColor(ImGuiCol.Text, Color.Gray.PackedValue);

					if (ImGui.Selectable(node.Name, selected))
					{
						selectedNode = node;
					}

					if (contextNode == node && contextOp == ContextOperation.Cut)
						ImGui.PopStyleColor();
				}

				if (ImGui.IsItemFocused())
					focused = true;

				if((ImGui.IsMouseDoubleClicked(ImGuiMouseButton.Left) ||
					ImGui.IsMouseClicked(ImGuiMouseButton.Middle)) && ImGui.IsItemHovered())
				{
					if (node.Item is IInteractableTreeViewItem inter)
						inter.Activate();
				}

				//ImGui.Unindent(ImGui.GetTreeNodeToLabelSpacing());

				ItemContextMenu(node);
			}
			else
			{
				ImGuiTreeNodeFlags flags = ImGuiTreeNodeFlags.OpenOnArrow | ImGuiTreeNodeFlags.SpanAvailWidth
					| ImGuiTreeNodeFlags.OpenOnDoubleClick;

				ImGui.SetNextItemOpen(node.Expanded);

				if (renameNode == node)
				{
					bool expand = ImGui.TreeNodeEx(string.Empty, flags);

					if (expand != node.Expanded)
					{
						if (ImGui.GetIO().KeyCtrl)
						{
							var n = GetDirectories(node).ToList();
							foreach (var dir in n)
								dir.Expanded = expand;
						}
					}

					node.Expanded = expand;

					ImGui.SameLine();
					DrawRenameNode();
				}
				else
				{
					if (contextNode == node && contextOp == ContextOperation.Cut)
						ImGui.PushStyleColor(ImGuiCol.Text, Color.Gray.PackedValue);

					if (selectedNode == node)
						flags |= ImGuiTreeNodeFlags.Selected;

					string text = $"{node.Name} ({node.GetItemCount()})";
					bool expand = ImGui.TreeNodeEx(text, flags);

					if (expand != node.Expanded)
					{
						if (ImGui.GetIO().KeyCtrl)
						{
							var n = GetDirectories(node).ToList();
							foreach (var dir in n)
								dir.Expanded = expand;
						}
					}

					node.Expanded = expand;

					if (contextNode == node && contextOp == ContextOperation.Cut)
						ImGui.PopStyleColor();

					if (ImGui.IsItemFocused())
						focused = true;

					if (ImGui.IsItemClicked())
					{
						selectedNode = node;
					}

					ItemContextMenu(node);
				}


				if (node.Expanded)
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

		private void DrawRenameNode()
		{
			AppGui.StrechNextItem();
			ImGui.InputText(string.Empty, ref renameText, 255,
								ImGuiInputTextFlags.CallbackCompletion | ImGuiInputTextFlags.AutoSelectAll);

			if (renameFocusRequest)
			{
				ImGui.SetKeyboardFocusHere();
				renameFocusRequest = false;
				return;
			}

			if (!ImGui.IsItemFocused() || AppGame.Gui.IsConfirmPressed)
			{
				if (!string.IsNullOrWhiteSpace(renameText) && renameNode.Name != renameText)
				{
					renameNode.Name = renameText;

					if (renameNode.UpdateItemPath(root))
						ItemModified = true;
				}
				renameNode = null;
			}
			if (AppGame.Gui.IsCancelPressed)
			{
				renameNode = null;
			}
		}

		private void ItemContextMenu(TreeItem node)
		{
			if (ImGui.BeginPopupContextItem())
			{
				selectedNode = node;

				if (node != root)
				{
					if (ImGui.Selectable("Rename"))
					{
						RenameNode(node);
						ImGui.CloseCurrentPopup();
					}
				}

				if (node.IsDir)
				{
					if (ImGui.Selectable("New Folder"))
					{
						var child = NewItem(node, new string[] { "New Folder" });
						node.Expanded = true;
						selectedNode = child;
						RenameNode(child);
						ImGui.CloseCurrentPopup();
					}
					if (NewItemAction != null)
					{
						string newItemName = $"New {ItemName}";

						if (ImGui.Selectable(newItemName))
						{
							var item = NewItemAction(null);
							if (node != root)
								item.Path = node.GetPath(root) + $"\\{newItemName}";
							else
								item.Path = newItemName;
							var child = NewItem(node, new string[] { newItemName }, item);
							node.Expanded = true;
							selectedNode = child;
							ItemModified = true;
							RenameNode(child);
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
							DuplicateNode(node);
							ImGui.CloseCurrentPopup();
						}

					}
				}

				if (node != root)
					if (ImGui.Selectable("Delete"))
					{
						DeleteNode(node);
						ImGui.CloseCurrentPopup();
					}
				ImGui.EndPopup();
			}
		}

		private TreeItem DuplicateNode(TreeItem node, TreeItem dst = null, bool rename = true)
		{
			dst = dst ?? node.Parent;
			if (dst != null && !dst.IsDir)
				dst = dst.Parent;

			if (dst == null || dst == root)
				return null;

			var item = NewItemAction(node.Item);
			string name = $"Copy of {node.Name}";
			if (dst != root)
				item.Path = dst.GetPath(root) + $"\\{name}";
			else
				item.Path = name;
			var child = NewItem(dst, new string[] { name }, item);
			ItemModified = true;
			selectedNode = child;

			if (rename)
				RenameNode(child);

			dst.Expanded = true;

			return child;
		}

		private void MoveNode(TreeItem node, TreeItem dst)
		{
			dst = dst ?? node.Parent;
			if (dst != null && !dst.IsDir)
				dst = dst.Parent;

			if (dst == null || dst == root)
				return;

			if (dst == node.Parent)
				return;


			node.Parent.Children.Remove(node);
			dst.Children.Add(node);
			node.Parent = dst;
			dst.UpdateItemPath(root);

			ItemModified = true;
			selectedNode = node;

			dst.Expanded = true;

		}

		private void RenameNode(TreeItem node)
		{
			renameNode = node;
			renameText = renameNode.Name;
			renameFocusRequest = true;
		}

		private void DeleteNode(TreeItem node)
		{
			if (DeleteItemAction == null)
				return;

			List<ITreeViewItem> items = new List<ITreeViewItem>();
			GetItemsInChildren(node, items);

			node.Parent.Children.Remove(node);
			foreach (var item in items)
				DeleteItemAction(item);
			if (items.Any())
				ItemModified = true;

			if (!NodeExists(selectedNode, root))
				selectedNode = null;

			if (!NodeExists(contextNode, root))
				contextNode = null;
		}
	}
}