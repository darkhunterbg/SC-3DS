using DataManager.Assets;
using DataManager.Panels;
using ImGuiNET;
using Microsoft.Xna.Framework.Input;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Numerics;
using System.Reflection;
using System.Text;
using System.Threading.Tasks;

namespace DataManager.Widgets
{
    public static class EditorModalSelect
    {
        private static Type objType;
        private static object selected;
        private static Action<object> callback;
        private static string textFilter = string.Empty;
        private static int focus = 0;

        public static void SelectItemModal(PropertyInfo info, object obj, Action onChanged)
        {
            SelectItemModal(
                info.PropertyType,
                info.GetValue(obj),
                (o) => { info.SetValue(obj, o); onChanged(); });
        }

        public static void SelectItemModal(Type objType, object selected, Action<object> selectionCompleted)
        {
            EditorModalSelect.objType = objType;
            EditorModalSelect.selected = selected;
            callback = selectionCompleted;
            focus = 5;
        }

        public static void DrawSelectItemModal()
        {
            bool opened = objType != null;

            if (!opened)
                return;

            ImGui.OpenPopup("modal.select");

            if (!ImGui.BeginPopupModal("modal.select", ref opened, ImGuiWindowFlags.NoTitleBar | ImGuiWindowFlags.AlwaysAutoResize))
                return;


            Header();

            ImGui.InputText("##modal.select.filter", ref textFilter, 255, ImGuiInputTextFlags.AutoSelectAll);
            if (focus > 0)
            {
                ImGui.SetKeyboardFocusHere();
                --focus;
                //focus = false;
                //ImGui.EndPopup();
                //return;

            }
            ImGui.BeginChild("modal.select.items", new Vector2(800, 800));

            int iter = 0;
            if (AppGame.Gui.IsButtonPressed(Keys.Up))
                --iter;
            if (AppGame.Gui.IsButtonPressed(Keys.Down))
                ++iter;
            Content(iter);

            ImGui.EndChild();

            if (ImGui.Button("Cancel##modal.select.cancel") || AppGame.Gui.IsCancelPressed)
            {
                objType = null;
                selected = null;
                textFilter = string.Empty;

            }

            ImGui.SameLine();

            if (selected != null)
            {
                if (ImGui.Button("Ok##modal.select.ok") || AppGame.Gui.IsConfirmPressed)
                {
                    callback(selected);

                    objType = null;
                    selected = null;
                    textFilter = string.Empty;
                }
            }
            ImGui.EndPopup();
        }

        private static void Header()
        {
            if (objType.IsSubclassOf(typeof(Asset)))
            {
                var asset = selected as Asset;

                if (asset?.Preview != null)
                {

                    ImGui.Image(asset.Preview.GuiImage, new Vector2(32, 32));

                }
                ImGui.SameLine();
                ImGui.Text(asset?.AssetName ?? string.Empty);
                return;
            }
            if (objType == typeof(IconRef))
            {
                if (selected == null)
                    ImGui.Text(string.Empty);
                else
                {
                    int id = ((IconRef)selected).Id;
                    string text = id != -1 ? id.ToString() : string.Empty;
                    ImGui.Text(text);
                }
                return;
            }
            if (objType == typeof(ImageListRef))
            {
                if (selected == null)
                    ImGui.Text(string.Empty);
                else
                {

                    string id = ((ImageListRef)selected).Key;
                    ImGui.Text(string.IsNullOrEmpty(id) ? "None" : id);
                }
                return;
            }

            if (objType == typeof(ImageFrameRef))
            {
                if (selected == null)
                    ImGui.Text(string.Empty);
                else
                {
                    var obj = ((ImageFrameRef)selected);
                    if (obj.Frame.Image != null)
                    {
                        ImGui.Image(obj.Frame.Image.GuiImage, new Vector2(32, 32));
                        ImGui.SameLine();
                    }
                    string id = obj.Key;
                    ImGui.Text(string.IsNullOrEmpty(id) ? "None" : id);
                }
                return;
            }
        }
        private static void Content(int moveIter)
        {
            if (objType.IsSubclassOf(typeof(Asset)))
            {
                AssetContent(moveIter);
                return;
            }

            if (objType == typeof(IconRef))
            {
                IconContent(moveIter);
                return;
            }

            if (objType == typeof(ImageListRef))
            {
                ImageListContent(moveIter);
                return;
            }
        }

        private static void AssetContent(int moveIter)
        {
            var tmp = selected as Asset;

            var query = AppGame.AssetManager.Assets[objType].Assets;
            query = Util.TextFilter(query, textFilter, t => t.AssetName);
            query = query.OrderBy(asset => asset.AssetName);

            int i = 0;

            foreach (var asset in query)
            {
                ImGui.PushID(i++);

                bool isSelectedItem = tmp == asset;

                if (ImGui.Selectable(string.Empty, isSelectedItem))
                {
                    if (isSelectedItem)
                        selected = null;
                    else
                        selected = asset;

                }
                if (asset.Preview != null)
                {
                    ImGui.SameLine();
                    ImGui.Image(asset.Preview.GuiImage, new Vector2(32, 32));

                }

                ImGui.SameLine();
                ImGui.Text(asset.AssetName);
                if (ImGui.IsItemHovered())
                {
                    AppGame.Gui.HoverObject = asset;

                }

                ImGui.PopID();
            }
        }

        private static void IconContent(int moveIter)
        {
            var icon = selected != null ? (IconRef)selected : IconRef.None;
            var query = AppGame.AssetManager.Icons;

            int i = 0;

            int? filter = null;

            if (int.TryParse(textFilter, out int x))
                filter = x;

            foreach (var asset in query)
            {
                if (filter.HasValue)
                {
                    if (asset.FrameIndex != filter)
                        continue;
                }

                i++;
                bool isSelectedItem = icon.Image == asset;

                Vector4 color = isSelectedItem ? Vector4.One : Vector4.Zero;

                if (ImGui.ImageButton(asset.Image.GuiImage, new Vector2(64, 64), Vector2.Zero,
            Vector2.One, 2,
            color,
            Microsoft.Xna.Framework.Color.Yellow.ToVec4()))
                {
                    if (isSelectedItem)
                        selected = null;
                    else
                        selected = new IconRef(asset);
                }


                if (i % 10 != 0)
                    ImGui.SameLine();
            }
        }

        private static void ImageListContent(int moveIter)
        {
            var tmp = selected != null ? (ImageListRef)selected : ImageListRef.None;


            IEnumerable<ImageList> query = AppGame.AssetManager.ImageLists;
            query = Util.TextFilter(query, textFilter, t => t.Key);
            query = query.OrderBy(asset => asset.Key);

            int selectedIndex = -1;
            if (tmp.Image != null)
                selectedIndex = query.IndexOf(tmp.Image);

            if (moveIter != 0)
            {
                selectedIndex += moveIter;
                selectedIndex %= query.Count();

                var s = query.Skip(selectedIndex).FirstOrDefault();
                if (s == null)
                    selectedIndex = -1;

                selected = tmp = s == null ? ImageListRef.None :
                    new ImageListRef(s);
            }

            int i = 0;

            foreach (var asset in query)
            {
                ImGui.PushID(i++);

                bool isSelectedItem = tmp.Key == asset.Key;

                if (ImGui.Selectable(string.Empty, isSelectedItem))
                {
                    if (isSelectedItem)
                        selected = ImageListRef.None;
                    else
                    {
                        selected = new ImageListRef(asset);
                    }
                }

                if (ImGui.IsItemHovered())
                {
                    AppGame.Gui.HoverObject = asset;
                }

                ImGui.SameLine();
                ImGui.Text(asset.Key);


                ImGui.PopID();
            }
        }
    }
}
