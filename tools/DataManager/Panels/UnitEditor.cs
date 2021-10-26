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
            RootName = "Units",
            DataSource = AppGame.AssetManager.GetAssets<UnitAsset>(),
        };
        private TablePropertyEditor propertyEditor = new TablePropertyEditor("propertyEditor");
        private SpriteView spriteView = new SpriteView()
        {
            ShowSelection = true,
            ShowBars = true,
            ShowShadows = true,
        };

        public UnitEditor()
        {
            tree.NewItemAction = NewItem;
            tree.DeleteItemAction = DeleteItem;
            AppGame.AssetManager.OnAssetsReloaded += () => tree.DataSource = AppGame.AssetManager.GetAssets<UnitAsset>();
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
            ImGui.Columns(3, "columns");
            ImGui.BeginChild("items");
            {
                tree.Draw();
                propertyEditor.EditingItem = tree.Selected;
                var selected = (tree.Selected as UnitAsset);
                spriteView.Sprite = selected?.Sprite;
                spriteView.SelectionType = selected?.SelectonSize ?? -1;
                spriteView.SelectionOffset = selected?.SelectionOffset ?? 0;
                spriteView.BarSize = selected?.BarSize ?? 0;
                spriteView.BarOffset = selected?.BarOffset ?? 0;
                spriteView.ShadowImage = selected?.Shadow ?? ImageListRef.None;
                spriteView.ShadowOffset = selected?.ShadowOffset ?? Vector2.Zero;
            }
            ImGui.EndChild();

            ImGui.NextColumn();

            ImGui.BeginChild("properties");
            propertyEditor.Draw();
            ImGui.EndChild();

            ImGui.NextColumn();

            ImGui.BeginChild("preview");
            spriteView.Draw(ImGui.GetColumnWidth());
            ImGui.EndChild();

            if (tree.ItemModified)
                AppGame.AssetManager.SaveAllAssets();
            else if (propertyEditor.Changed)
                AppGame.AssetManager.GetAssetDatabase<UnitAsset>().Save();
        }
    }
}
