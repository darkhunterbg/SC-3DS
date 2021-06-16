using DataManager.Assets;
using DataManager.Assets.Raw;
using DataManager.Panels;
using Microsoft.Xna.Framework.Graphics;
using System;
using System.Collections;
using System.Collections.Generic;
using System.Linq;
using System.Numerics;
using System.Text;
using System.Threading;
using System.Threading.Tasks;
using Rectangle = Microsoft.Xna.Framework.Rectangle;
using Color = Microsoft.Xna.Framework.Color;
using CsvHelper;
using System.IO;
using System.Diagnostics;
using DataManager.Gameplay;

namespace DataManager.Build
{
    public class BuildGenerator : IDisposable
    {
        private List<SpriteAtlasEntry> atlases = null;

        private volatile bool cancelled = false;

        public bool DrawAtlasOutiline { get; set; }

        int totalJobs = 0;
        int currentJob = 0;

        private AsyncOperation op;

        private void DisplayItem(string text)
        {
            op.ItemName = $"{text} {currentJob}/{totalJobs}";
        }
        private void Progress(int current, int max)
        {
            op.Progress = (float)(current) / (float)max;
        }

        public List<SpriteAtlas> GeneratedAtlases { get; private set; } = new List<SpriteAtlas>();
        public List<SpriteSubAtlas> GeneratedSubAtlases { get; private set; } = new List<SpriteSubAtlas>();
        public List<ImageFrameAtlasData> GeneratedFrames { get; private set; } = new List<ImageFrameAtlasData>();
        public List<ImageListAtlasData> GeneratedImages { get; private set; } = new List<ImageListAtlasData>();

        private SpriteBatch spriteBatch = new SpriteBatch(AppGame.Device);

        public AsyncOperation BuildAtlases(List<SpriteAtlasEntry> atlases)
        {
            this.atlases = atlases;

            Directory.Delete(AssetManager.SpriteAtlasOutDir, true);
            Directory.Delete(AssetManager.SpriteAtlas3DSBuildDir, true);

            Directory.CreateDirectory(AssetManager.SpriteAtlasOutDir);
            Directory.CreateDirectory(AssetManager.SpriteAtlas3DSBuildDir);

            totalJobs = atlases.Count;

            return new AsyncOperation(BuildAsync, OnCancelAsync);
        }

        private void BuildAsync(AsyncOperation op)
        {
            this.op = op;
            totalJobs = atlases.Count;
            foreach (var atlas in atlases)
            {
                ++currentJob;

                DisplayItem(atlas.OutputName);

                List<AtlasBSPTree> subAtlases = GenerateAtlasTree(atlas);

                if (cancelled)
                    return;

                List<AtlasBSPTree> scaledSubAtlases = SimplifyAtlasTrees(subAtlases);

                var crt = AppGame.RunCoroutine(SaveAtlasTextureCrt(scaledSubAtlases, atlas));

                while (!crt.done && !cancelled)
                    Thread.Sleep(1);

            }


            if (cancelled)
                return;

            SaveAtlasTable();

            if (cancelled)
                return;

            //BuildT3XFiles();

            if (cancelled)
                return;

            BuildData();
        }

        private static List<AtlasBSPTree> SimplifyAtlasTrees(List<AtlasBSPTree> subAtlases)
        {
            List<AtlasBSPTree> scaledSubAtlases = new List<AtlasBSPTree>();

            foreach (var subAtlas in subAtlases)
            {
                var tree = subAtlas;

                while (tree.TakenSpace < (tree.TotalSpace / 4) && tree.TotalSpace >= 128 * 128)
                {
                    bool success = true;

                    var test = new AtlasBSPTree(tree.Dimensions.X / 2, tree.Dimensions.Y / 2);
                    var frames = tree.GetFrames();

                    foreach (var f in frames)
                    {
                        if (!test.TryAdd(f.image))
                        {
                            success = false;
                            break;
                        }
                    }
                    if (!success)
                        break;

                    tree = test;
                }

                scaledSubAtlases.Add(tree);
            }

            return scaledSubAtlases;
        }

        private void OnCancelAsync()
        {
            cancelled = true;
        }

        private IEnumerator SaveAtlasTextureCrt(List<AtlasBSPTree> subAtlases, SpriteAtlasEntry atlas)
        {
            SpriteAtlas generated = new SpriteAtlas(atlas.OutputName);
            GeneratedAtlases.Add(generated);

            int i = 0;

            foreach (var subAtlas in subAtlases)
            {
                if (cancelled)
                    yield break;

                var tree = subAtlas;

                using (var texture = new RenderTarget2D(AppGame.Device, tree.Dimensions.X, tree.Dimensions.Y))
                {

                    texture.GraphicsDevice.SetRenderTarget(texture);
                    texture.GraphicsDevice.Clear(Color.Transparent);

                    var frames = tree.GetFrames();

                    spriteBatch.Begin(samplerState: SamplerState.PointClamp);

                    foreach (var f in frames)
                    {
                        spriteBatch.Draw(f.image.Image.Texture, f.rect.Location.ToVector2(), Color.White);
                    }
                    if (DrawAtlasOutiline)
                    {
                        foreach (var f in tree.GetNodes())
                        {
                            spriteBatch.DrawRectangle(f.Region.Location.ToVector2().ToVec2(),
                new Vector2(1, f.Region.Size.Y),
                 Color.Magenta);


                            spriteBatch.DrawRectangle(f.Region.Location.ToVector2().ToVec2()
                                + new Vector2(f.Region.Size.X - 1, 0),
                            new Vector2(1, f.Region.Size.Y),
                             Color.Magenta);

                            spriteBatch.DrawRectangle(f.Region.Location.ToVector2().ToVec2(),
                                new Vector2(f.Region.Size.X, 1),
                                 Color.Magenta);

                            spriteBatch.DrawRectangle(f.Region.Location.ToVector2().ToVec2()
                                + new Vector2(0, f.Region.Size.Y - 1),
                            new Vector2(f.Region.Size.X, 1),
                             Color.Magenta);
                        }
                    }

                    spriteBatch.End();

                    generated.AddSubAtlas(frames.Select(s =>
                    new ImageFrameAtlasData(s.image, s.rect.Location.ToVector2().ToVec2()))
                        .ToList());

                    texture.SaveAsPng($"{AssetManager.SpriteAtlasOutDir}{generated.SubAtlases.Last().FullName}.png");

                }

                i++;
                yield return null;
            }
        }

        private ImageList FindFittingImageList(List<ImageList> assets, AtlasBSPTree tree, out AtlasBSPTree packedTree)
        {
            packedTree = null;

            foreach (var imageList in assets.Where(t => t.TakenSpace <= tree.FreeSpace))
            {
                if (cancelled)
                    return null;

                AtlasBSPTree test = tree.Clone();
                bool success = true;

                foreach (var frame in imageList.Frames.OrderByDescending(t => t.rect.Size.Y)
                    .ThenBy(t => t.rect.Size.X))
                {
                    if (!test.TryAdd(frame))
                    {
                        success = false;
                        break;
                    }
                }

                if (success)
                {
                    packedTree = test;
                    return imageList;
                }
            }

            return null;
        }

        private List<AtlasBSPTree> GenerateAtlasTree(SpriteAtlasEntry atlas)
        {
            var assets = atlas.GetImageListsForBuild();
            foreach (var asset in assets)
            {
                GeneratedImages.Add(new ImageListAtlasData(asset));
            }

            int assetTotal = assets.Count;
            int assetProgress = 1;

            AtlasBSPTree current = new AtlasBSPTree();

            List<AtlasBSPTree> subAtlases = new List<AtlasBSPTree>();


            if (atlas.PackStrategy == SpriteAtlasPackStrategy.TightPacking)
                goto TightPacking;


            while (assets.Count > 0)
            {
                var add = FindFittingImageList(assets, current, out var packed);
                if (cancelled)
                    return null;

                if (add != null)
                {
                    assets.Remove(add);

                    current = packed;

                    ++assetProgress;
                    Progress(assetProgress, assetTotal);
                }
                else
                {
                    if (current.IsEmpty())
                    {
                        // Cant  find anything to fit in empty, remaining should be tighlty-apcked
                        break;
                    }
                    subAtlases.Add(current);
                    current = new AtlasBSPTree();
                }
            }

            if (!current.IsEmpty())
            {
                subAtlases.Add(current);
                current = new AtlasBSPTree();
            }

        TightPacking:
            foreach (var imageList in assets)
            {
                if (cancelled)
                    return subAtlases;

                foreach (var frame in imageList.Frames.OrderByDescending(t => t.rect.Size.Y)
                    .ThenBy(t => t.rect.Size.X))
                {
                    bool success = false;

                    foreach (var sub in subAtlases.Where(t => t.FreeSpace >= frame.TakenSpace))
                    {
                        if (sub.TryAdd(frame))
                        {
                            success = true;
                            break;
                        }
                    }


                    if (!success)
                    {
                        var newAtlas = new AtlasBSPTree();
                        newAtlas.TryAdd(frame);
                        subAtlases.Add(newAtlas);
                    }
                }


                ++assetProgress;
                Progress(assetProgress, assetTotal);

            }

            if (!current.IsEmpty())
            {
                subAtlases.Add(current);
                current = new AtlasBSPTree();
            }

            return subAtlases;
        }

        private void SaveAtlasTable()
        {
            GeneratedImages = GeneratedImages.OrderBy(t => t.Name).ToList();

            GeneratedFrames = GeneratedAtlases.SelectMany(s => s.SubAtlases).SelectMany(s => s.Images)
                .OrderBy(s => s.Frame.ImageListName).ThenBy(s => s.Frame.FrameIndex).ToList();

            GeneratedSubAtlases = GeneratedAtlases.SelectMany(s => s.SubAtlases).OrderBy(a => a.AtlasName)
        .ThenBy(a => a.AtlasIndex).ToList();

            foreach (var img in GeneratedImages)
            {
                img.FrameOffset = GeneratedFrames.FindIndex(i => i.Frame.ImageList == img.List);
            }

            int i = 0;
            foreach (var subAtlases in GeneratedSubAtlases)
            {
                foreach (var f in subAtlases.Images)
                    f.AtlasId = i;

                ++i;
            }

            using (var csv = new CsvWriter(new StreamWriter(AssetManager.ImagesDataPath), AssetManager.CsvConfig))
            {
                csv.WriteRecords(GeneratedImages);
            }


            using (var csv = new CsvWriter(new StreamWriter(AssetManager.FramesDataPath), AssetManager.CsvConfig))
            {
                csv.WriteRecords(GeneratedFrames);
            }



            using (var csv = new CsvWriter(new StreamWriter(AssetManager.SpriteAtlasDataPath), AssetManager.CsvConfig))
            {
                csv.WriteRecords(GeneratedSubAtlases);
            }
        }

        private void BuildT3XFiles()
        {
            var subAtlases = GeneratedAtlases.SelectMany(s => s.SubAtlases).ToList();

            currentJob = 0;
            totalJobs = subAtlases.Count();

            Progress(currentJob, totalJobs);

            foreach (var atlas in subAtlases)
            {
                if (cancelled)
                    return;

                currentJob++;

                DisplayItem($"{atlas.FullName}.t3x");

                string inputTexture = Path.GetFullPath($"{AssetManager.SpriteAtlasOutDir}{atlas.FullName}") + ".png";
                string outAtlas = Path.GetFullPath(Path.Combine(AssetManager.SpriteAtlas3DSBuildDir, atlas.FullName)) + ".t3x";

                string args = $" -f auto-etc1 -z auto -q high -o {outAtlas} {inputTexture}";
                var process = new ProcessStartInfo(AssetManager.tex3dsPath, args);
                process.UseShellExecute = false;
                process.CreateNoWindow = true;
                process.RedirectStandardOutput = true;
                process.RedirectStandardError = true;
                var p = new Process()
                {
                    StartInfo = process
                };


                p.Start();

                p.WaitForExit();

                string error = p.StandardError.ReadToEnd();
                if (!string.IsNullOrEmpty(error))
                    throw new Exception($"Failed to build atlas {atlas.FullName}: {error}");


                Progress(currentJob, totalJobs);
            }
        }

        private void BuildData()
        {
            List<AnimClipInstructionData> instructionData = new List<AnimClipInstructionData>();

            foreach (var clip in AppGame.AssetManager.GetAssets<SpriteAnimClipAsset>()
                .OrderBy(s => s.Sprite.AssetName).ThenBy(s => s.Type))
            {
                clip._InstructionStart = (uint)instructionData.Count;

                instructionData.AddRange(clip.Instructions.Select(s => new AnimClipInstructionData(clip, s)));
            }

            //foreach(var asset in AppGame.AssetManager.GetAssetDatabase<SpriteAsset>().Assets)
            //{
            //    asset._ImageListIndex = GeneratedImages.IndexOf(i => i.List == asset.Image.Image);
            //}

            List<DataItem> data = new List<DataItem>();
            data.Add(new DataItem()
            {
                Name = "Atlases",
                Data = GeneratedSubAtlases,
            });
            data.Add(new DataItem()
            {
                Name = "Images",
                Data = GeneratedImages,
            });
            data.Add(new DataItem()
            {
                Name = "Frames",
                Data = GeneratedFrames,
            });
            data.Add(new DataItem()
            {
                Name = "Animation Instructions",
                Data = instructionData,
            });


            foreach (var assetDb in AppGame.AssetManager.Assets.Values)
            {
                assetDb.PrepareForSerialization();
                data.Add(new DataItem()
                {
                    Name = assetDb.Type.Name,
                    Data = assetDb.Assets
                });
            }

            foreach (var d in data)
                d.Init();

            currentJob = 0;
            totalJobs = data.Count;

            Progress(currentJob, totalJobs);

            DataItemHeader[] headers = data.Select(s => s.GetHeader()).ToArray();


            uint offset = (uint)(headers.Length * DataItemHeader.Size);
            offset += 4;

            List<MemoryStream> binData = new List<MemoryStream>();

            foreach (var item in data)
            {
                headers[currentJob].Offset = offset;

                ++currentJob;
                DisplayItem(item.Name);

                var stream = item.MemorySerialize();
                binData.Add(stream);

                offset += (uint)stream.Length;

                Progress(currentJob, totalJobs);
            }

            byte[] binHeaders = headers.SelectMany(s => s.Serialize()).ToArray();
            currentJob = 0;
            totalJobs = data.Count;


            using (FileStream stream = new FileStream(AssetManager.BuildDataFile, FileMode.Create))
            {
                stream.Write(BitConverter.GetBytes((uint)headers.Length));

                DisplayItem("Data Headers");
                stream.Write(binHeaders);

                DisplayItem("Data Sections");

                foreach (var str in binData)
                {
                    str.Position = 0;
                    ++currentJob;
                    byte[] bin = new byte[str.Length];
                    str.Read(bin, 0, (int)str.Length);
                    stream.Write(bin);
                    Progress(currentJob, totalJobs);
                }
            }
        }

        public void Dispose()
        {

        }
    }
}
