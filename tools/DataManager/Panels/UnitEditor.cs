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
    public class UnitEditor : IGuiPanel
    {
        public string WindowName => "Unit Editor";

        private TreeView tree = new TreeView("tree")
        {
            ItemName = "Unit",
            DataSource = AppGame.AssetManager.GetAssets<UnitAsset>()
        };
        private TablePropertyEditor propertyEditor = new TablePropertyEditor("propertyEditor");


        public UnitEditor()
        {
            tree.NewItemAction = NewItem;
            tree.DeleteItemAction = DeleteItem;
        }

        private ITreeViewItem NewItem(ITreeViewItem item)
        {
            var asset = item as UnitAsset;
            return AppGame.AssetManager.GetAssetDatabase<UnitAsset>().New(asset);
        }

        private void DeleteItem(ITreeViewItem item)
        {
            var asset = item as UnitAsset;
            AppGame.AssetManager.GetAssetDatabase<UnitAsset>().Delete(asset);
        }

        public void Draw(Vector2 client)
        {
            ImGui.Columns(2, "columns");
            ImGui.BeginChild("items");
            {
                tree.Draw();
            }
            ImGui.EndChild();
            ImGui.NextColumn();
            ImGui.BeginChild("properties");
            propertyEditor.EditingItem = tree.Selected;
            propertyEditor.Draw();
            ImGui.EndChild();

            if (tree.ItemModified || propertyEditor.Changed)
                AppGame.AssetManager.GetAssetDatabase<UnitAsset>().Save();
        }
    }
}
