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

namespace DataManager.Build
{
	class AtlasBSPTree
	{
		const int MaxTextureSize = 1024;

		public class AtlasFrame
		{
			public Rectangle rect;
			public ImageFrame image;
		}

		public class Node
		{
			public Rectangle Region;

			public AtlasFrame Frame = null;

			public Node[] Children = null;

			public bool IsXSplit = false;

			public Node Clone()
			{
				Node n = new Node()
				{
					Frame = Frame,
					IsXSplit = IsXSplit,
					Region = Region

				};
				if (Children != null)
				{
					n.Children = new Node[Children.Length];

					for (int i = 0; i < Children.Length; ++i)
					{
						n.Children[i] = Children[i].Clone();
					}
				}

				return n;
			}
		}

		Node root;

		int frames = 0;

		public int FreeSpace { get; private set; }

		public int TotalSpace => root.Region.Width * root.Region.Height;

		public int TakenSpace => TotalSpace - FreeSpace;

		public Microsoft.Xna.Framework.Point Dimensions => root.Region.Size;

		bool TryAdd(ImageFrame image, Node node)
		{
			Rectangle rect = node.Region;
			rect.Size = image.rect.Size;
			rect.Location = node.Region.Location;

			if (!node.Region.Contains(rect))
				return false;

			if (node.Children == null)
			{
				SplitNodeX(node, rect.Height + 1);
				SplitNodeY(node.Children[0], rect.Width + 1);

				node.Children[0].Children[0].Frame = new AtlasFrame()
				{
					rect = rect,
					image = image
				};

				++frames;
				FreeSpace -= (rect.Width + 1) * (rect.Height + 1);
				return true;
			}

			foreach (var child in node.Children)
			{
				if (child.Frame != null)
					continue;

				if (TryAdd(image, child))
					return true;
			}

			return false;
		}

		private void SplitNodeX(Node node, int y)
		{
			node.IsXSplit = true;
			node.Children = new Node[2] { new Node(), new Node() };
			node.Children[0].Region = node.Region;
			node.Children[0].Region.Height = y;
			node.Children[1].Region = node.Region;
			node.Children[1].Region.Y += y;
			node.Children[1].Region.Height -= y;
		}

		private void SplitNodeY(Node node, int x)
		{
			node.IsXSplit = false;
			node.Children = new Node[2] { new Node(), new Node() };
			node.Children[0].Region = node.Region;
			node.Children[0].Region.Width = x;
			node.Children[1].Region = node.Region;
			node.Children[1].Region.X += x;
			node.Children[1].Region.Width -= x;
		}

		private bool IsEmpty(Node node)
		{
			if (node.Frame != null)
				return false;

			if (node.Children != null)
			{
				foreach (var c in node.Children)
					if (!IsEmpty(c))
						return false;
			}

			return true;
		}

		public bool IsEmpty() => IsEmpty(root);

		private void GetNodes(Node node, List<Node> result)
		{
			result.Add(node);

			if (node.Children != null)
			{
				foreach (var c in node.Children)
					GetNodes(c, result);
			}
		}

		public List<Node> GetNodes()
		{
			List<Node> result = new List<Node>();
			GetNodes(root, result);

			return result;
		}

		private void GetFrames(Node node, List<AtlasFrame> result)
		{
			if (node.Frame != null)
			{
				result.Add(node.Frame);
			}

			if (node.Children != null)
			{
				foreach (var c in node.Children)
					GetFrames(c, result);
			}
		}

		public List<AtlasFrame> GetFrames()
		{
			List<AtlasFrame> result = new List<AtlasFrame>();
			GetFrames(root, result);

			return result;
		}

		public bool TryAdd(ImageFrame image)
		{
			return TryAdd(image, root);
		}

		public AtlasBSPTree Clone()
		{
			return new AtlasBSPTree()
			{
				root = root.Clone(),
				frames = frames,
				FreeSpace = FreeSpace
			};
		}

		public AtlasBSPTree(int width = MaxTextureSize, int height = MaxTextureSize)
		{
			root = new Node()
			{
				Region = new Rectangle(0, 0, width, height)
			};

			FreeSpace = width * height;
		}
	}


	public class BuildGenerator : IDisposable
	{
		private List<SpriteAtlasEntry> atlases = null;

		private volatile bool cancelled = false;

		public bool DrawAtlasOutiline { get; set; }

		int totalJobs = 0;
		int currentJob = 0;

		public List<SpriteAtlas> GeneratedAtlases { get; private set; } = new List<SpriteAtlas>();

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
			foreach (var atlas in atlases)
			{
				++currentJob;
				List<AtlasBSPTree> subAtlases = GenerateAtlasTree(atlas, op);

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

			BuildT3XFiles(op);
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

		private List<ImageList> GetUsedImageLists(SpriteAtlasEntry atlas)
		{
			List<ImageList> lists = new List<ImageList>();

			foreach(var assetDb in AppGame.AssetManager.Assets.Values)
			{
				var props = assetDb.Type.GetProperties().Where(p => p.PropertyType == typeof(ImageListRef)).ToList();

				foreach(var asset in assetDb.Assets)
				{
					foreach (var prop in props)
					{
						var value = prop.GetValue(asset);
						if (value == null)
							continue;

						var img = ((ImageListRef)value).Image;
						if (lists.Contains(img))
							continue;

						if (atlas.Assets.Contains(img))
							lists.Add(img);
					}
				}
			}
			
			 lists = lists
				.OrderByDescending(t => t.FrameSize.Y)
				.ThenBy(t => t.FrameSize.X)
				.ThenBy(t => t.TakenSpace).ToList();

			return lists;

		}

		private List<AtlasBSPTree> GenerateAtlasTree(SpriteAtlasEntry atlas, AsyncOperation op)
		{
			var assets = GetUsedImageLists(atlas);

			int assetTotal = assets.Count;
			int assetProgress = 1;

			AtlasBSPTree current = new AtlasBSPTree();

			List<AtlasBSPTree> subAtlases = new List<AtlasBSPTree>();

			op.ItemName = $"{atlas.OutputName} {assetProgress}/{totalJobs}";

			if (atlas.PackStrategy == SpriteAtlasPackStrategy.Tight)
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
					op.Progress = ((float)assetProgress / (float)assetTotal);
					op.ItemName = $"{atlas.OutputName} {currentJob}/{totalJobs}";
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
				op.Progress = ((float)assetProgress / (float)assetTotal);
				op.ItemName = $"{atlas.OutputName} {currentJob}/{totalJobs}";

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
			var frames = GeneratedAtlases.SelectMany(s => s.SubAtlases).SelectMany(s => s.Images)
				.OrderBy(s => s.ImageListName).ThenBy(s => s.FrameIndex);

			using (var csv = new CsvWriter(new StreamWriter(AssetManager.FramesDataPath), AssetManager.CsvConfig))
			{
				csv.WriteRecords(frames);
			}

			var atlases = GeneratedAtlases.SelectMany(s => s.SubAtlases).OrderBy(a => a.AtlasName)
				.ThenBy(a => a.AtlasIndex);

			using (var csv = new CsvWriter(new StreamWriter(AssetManager.SpriteAtlasDataPath), AssetManager.CsvConfig))
			{
				csv.WriteRecords(atlases);
			}
		}

		private void BuildT3XFiles(AsyncOperation op)
		{
			int i = 0;

			foreach (var atlas in GeneratedAtlases.SelectMany(s => s.SubAtlases))
			{
				if (cancelled)
					return;

				op.ItemName = $"{atlas.FullName}.t3x";

				op.Progress = i / (float)GeneratedAtlases.SelectMany(s => s.SubAtlases).Count();

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


				++i;
			}
		}
		public void Dispose()
		{

		}
	}
}
