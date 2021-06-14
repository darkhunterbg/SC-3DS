using ImGuiNET;
using Microsoft.Xna.Framework.Input;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Numerics;
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
        private TreeItem NextNode(TreeItem node)
        {
            var parent = node.Parent;
            if (parent != null)
            {
                int index = parent.ChildrenView.IndexOf(node);
                if (parent.Children.Count > index + 1)
                {
                    ++index;
                    return parent.ChildrenView.Skip(index).First();
                }
                if (node.Children.Count > 0 && node.Opened)
                    return node.ChildrenView.First();
                else
                    return NextNode(node.Parent);
            }
            if (node.Children.Count > 0 && node.Opened)
                return node.ChildrenView.First();
            return node;
        }
        private TreeItem PrevNode(TreeItem node)
        {
            var parent = node.Parent;
            if (parent != null)
            {
                int index = parent.ChildrenView.IndexOf(node);
                if (index > 0)
                {
                    --index;
                    return parent.ChildrenView.Skip(index).First();
                }
                return parent;
            }
            return node;
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

        private void ProcessInput()
        {
            if (AppGame.Gui.IsButtonPressed(Keys.F2))
                RenameNode(selectedNode);
            if (AppGame.Gui.IsButtonPressed(Keys.Delete))
                DeleteItem(selectedNode);

            if (AppGame.Gui.IsButtonPressed(Keys.D) && ImGui.GetIO().KeyCtrl)
            {
                if (Selected != null)
                    DuplicateNode(selectedNode);
            }

            if (AppGame.Gui.IsButtonPressed(Keys.Right))
            {
                if (!selectedNode.Opened && selectedNode.IsDir)
                    selectedNode.Opened = true;
                else if (selectedNode.Children.Count > 0)
                    selectedNode = selectedNode.Children[0];

            }

            if (AppGame.Gui.IsButtonPressed(Keys.Left))
            {
                if (selectedNode.Opened && selectedNode.IsDir)
                    selectedNode.Opened = false;
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
                    if (ImGui.Selectable(node.Name, selected))
                    {
                        selectedNode = node;
                    }
                }

                if (ImGui.IsItemFocused())
                    focused = true;
                //ImGui.Unindent(ImGui.GetTreeNodeToLabelSpacing());

                ItemContextMenu(node);
            }
            else
            {
                ImGuiTreeNodeFlags flags = ImGuiTreeNodeFlags.OpenOnArrow | ImGuiTreeNodeFlags.SpanAvailWidth
                    | ImGuiTreeNodeFlags.OpenOnDoubleClick;

                if (node.Opened)
                    flags |= ImGuiTreeNodeFlags.DefaultOpen;


                if (renameNode == node)
                {
                    node.Opened = ImGui.TreeNodeEx(string.Empty, flags);
                    ImGui.SameLine();
                    DrawRenameNode();
                }
                else
                {
                    if (selectedNode == node)
                        flags |= ImGuiTreeNodeFlags.Selected;

                    string text = $"{node.Name} ({node.Children.Count})";
                    node.Opened = ImGui.TreeNodeEx(text, flags);

                    if (ImGui.IsItemFocused())
                        focused = true;

                    if (ImGui.IsItemClicked())
                    {
                        selectedNode = node;
                    }

                    ItemContextMenu(node);
                }


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
                        node.Opened = true;
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
                            node.Opened = true;
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
                        DeleteItem(node);
                        ImGui.CloseCurrentPopup();
                    }
                ImGui.EndPopup();
            }
        }

        private void DuplicateNode(TreeItem node)
        {
            var item = NewItemAction(node.Item);
            string name = $"Copy of {node.Name}";
            if (node != root)
                item.Path = node.GetPath(root) + $"\\{name}";
            else
                item.Path = name;
            var child = NewItem(node.Parent, new string[] { name }, item);
            ItemModified = true;
            selectedNode = child;

            RenameNode(child);
        }

        private void RenameNode(TreeItem node)
        {
            renameNode = node;
            renameText = renameNode.Name;
            renameFocusRequest = true;
        }

        private void DeleteItem(TreeItem node)
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
        }
    }
}