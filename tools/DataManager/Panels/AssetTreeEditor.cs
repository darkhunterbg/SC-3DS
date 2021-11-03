using DataManager.Assets;
using DataManager.Widgets;
using ImGuiNET;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Numerics;
using System.Text;
using System.Threading.Tasks;

namespace DataManager.Panels
{
    public abstract class AssetTreeEditor<TAsset> : IGuiPanel
        where TAsset : Asset, ITreeViewItem, new()
    {
        public string WindowName { get; private set; }

        private TreeView tree = new TreeView("tree")
        {
            DataSource = AppGame.AssetManager.GetAssets<TAsset>(),
        };
        private TablePropertyEditor propertyEditor = new TablePropertyEditor($"{typeof(TAsset).Name}.propertyEditor");


        public AssetTreeEditor(string itemName, string rootName, string windowName)
        {
            WindowName = windowName;
            tree.ItemName = itemName;
            tree.RootName = rootName;
            tree.NewItemAction = NewItem;
            tree.DeleteItemAction = DeleteItem;

            AppGame.AssetManager.OnAssetsReloaded += () => tree.DataSource = AppGame.AssetManager.GetAssets<TAsset>();
        }

        private ITreeViewItem NewItem(ITreeViewItem item)
        {
            var asset = item as TAsset;
            return AppGame.AssetManager.GetAssetDatabase<TAsset>().New(asset);
        }

        private void DeleteItem(ITreeViewItem item)
        {
            var asset = item as TAsset;
            AppGame.AssetManager.GetAssetDatabase<TAsset>().Delete(asset);
        }

        public void Draw(Vector2 client)
        {

            ImGui.Columns(2, "columns");
            ImGui.BeginChild("items");
            {
                tree.Draw();
                propertyEditor.EditingItem = tree.Selected;
            }
            ImGui.EndChild();

            ImGui.NextColumn();

            ImGui.BeginChild("properties");
            propertyEditor.Draw();
            ImGui.EndChild();

            if (tree.ItemModified)
                AppGame.AssetManager.SaveAllAssets();
            else
            if (propertyEditor.Changed)
                AppGame.AssetManager.GetAssetDatabase<TAsset>().Save();
        }
    }
}
