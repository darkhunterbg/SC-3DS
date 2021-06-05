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

namespace DataManager.Build
{
	class AtlasBSPTree
	{
		public class AtlasFrame
		{
			public Rectangle rect;
			public GuiTexture texture;
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

		Node root = new Node()
		{
			Region = new Rectangle(0, 0, 1024, 1024)
		};

		int frames = 0;

		bool TryAdd(GuiTexture texture, Node node)
		{
			Rectangle rect = node.Region;
			rect.Size = new Microsoft.Xna.Framework.Point(texture.Texture.Width, texture.Texture.Height);
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
					texture = texture
				};

				++frames;
				return true;
			}

			foreach (var child in node.Children)
			{
				if (child.Frame != null)
					continue;

				if (TryAdd(texture, child))
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

		public bool TryAdd(GuiTexture texture)
		{
			return TryAdd(texture, root);
		}

		public AtlasBSPTree Clone()
		{
			return new AtlasBSPTree()
			{
				root = root.Clone(),
				frames = frames
			};
		}

	}

	public class BuildGenerator : IDisposable
	{
		private List<SpriteAtlasEntry> atlases = null;

		private volatile bool cancelled = false;

		private RenderTarget2D texture = new RenderTarget2D(AppGame.Device, 1024, 1024, false, SurfaceFormat.Color, DepthFormat.None);

		private SpriteBatch spriteBatch = new SpriteBatch(AppGame.Device);

		public AsyncOperation BuildAtlases(List<SpriteAtlasEntry> atlases)
		{
			this.atlases = atlases;

			return new AsyncOperation(BuildAsync, OnCancelAsync);
		}

		private void OnCancelAsync()
		{
			cancelled = true;
		}

		private IEnumerator SaveAtlasCrt(List<AtlasBSPTree> subAtlases, SpriteAtlasEntry atlas)
		{

			int i = 0;
			foreach (var tree in subAtlases)
			{
				if (cancelled)
					yield break;

				texture.GraphicsDevice.SetRenderTarget(texture);
				texture.GraphicsDevice.Clear(Microsoft.Xna.Framework.Color.Transparent);

				var frames = tree.GetFrames();

				spriteBatch.Begin(samplerState: SamplerState.PointClamp);

				foreach (var f in frames)
				{
					spriteBatch.Draw(f.texture.Texture, f.rect.Location.ToVector2(), Microsoft.Xna.Framework.Color.White);
				}

				foreach (var f in tree.GetNodes()){

					spriteBatch.DrawRectangle(f.Region.Location.ToVector2().ToVec2(),
				new Vector2(1, f.Region.Size.Y),
				 Microsoft.Xna.Framework.Color.Magenta);

					spriteBatch.DrawRectangle(f.Region.Location.ToVector2().ToVec2()
						+ new Vector2(f.Region.Size.X - 1, 0),
					new Vector2(1, f.Region.Size.Y),
					 Microsoft.Xna.Framework.Color.Magenta);

					spriteBatch.DrawRectangle(f.Region.Location.ToVector2().ToVec2(),
						new Vector2(f.Region.Size.X, 1),
						 Microsoft.Xna.Framework.Color.Magenta);

					spriteBatch.DrawRectangle(f.Region.Location.ToVector2().ToVec2()
						+ new Vector2(0, f.Region.Size.Y - 1),
					new Vector2(f.Region.Size.X, 1),
					 Microsoft.Xna.Framework.Color.Magenta);
				}

				spriteBatch.End();

				texture.SaveAsPng($"{AssetManager.SpriteAtlasDir}gen\\{atlas.OutputName}_{i++}.png");

				yield return null;
			}


		}

		private void BuildAsync(AsyncOperation op)
		{
			int total = atlases.Sum(s => s.Assets.Count);
			int progress = 0;

			List<ImageList> problematic = new List<ImageList>();

			foreach (var atlas in atlases)
			{
				op.ItemName = atlas.OutputName;

				List<AtlasBSPTree> subAtlases = new List<AtlasBSPTree>();

				AtlasBSPTree current = new AtlasBSPTree();

				problematic.Clear();

				foreach (var imageList in atlas.Assets.OrderByDescending(t => t.FrameSize.X * t.FrameSize.Y))
				{
					if (cancelled)
						return;


					AtlasBSPTree test = current.Clone();
					bool success = true;


					foreach (var frame in imageList.Frames.OrderByDescending(t => t.rect.Size.X * t.rect.Size.Y))
					{
						var texture = AppGame.AssetManager.GetSheetImage(imageList.RelativePath, frame.index);

						if (!test.TryAdd(texture))
						{
							success = false;
							break;
						}
					}

					if (!success)
					{
						subAtlases.Add(current);
						current = new AtlasBSPTree();
						test = new AtlasBSPTree();

						success = true;

						foreach (var frame in imageList.Frames)
						{
							var texture = AppGame.AssetManager.GetSheetImage(imageList.RelativePath, frame.index);

							if (!test.TryAdd(texture))
							{
								problematic.Add(imageList);
								success = false;
								break;
							}
						}
					}

					if (success)
					{
						current = test;
						progress++;
						op.Progress = ((float)progress / (float)total);
					}
				}

				foreach (var imageList in problematic)
				{
					if (cancelled)
						return;

					foreach (var frame in imageList.Frames.OrderByDescending(t => t.rect.Size.X * t.rect.Size.Y))
					{
						var texture = AppGame.AssetManager.GetSheetImage(imageList.RelativePath, frame.index);

						if (!current.TryAdd(texture))
						{
							subAtlases.Add(current);
							current = new AtlasBSPTree();
							current.TryAdd(texture);
						}
					}

					progress++;
					op.Progress = ((float)progress / (float)total);

				}

				if (!current.IsEmpty())
				{
					subAtlases.Add(current);
					current = new AtlasBSPTree();
				}

				var crt = AppGame.RunCoroutine(SaveAtlasCrt(subAtlases, atlas));

				while (!crt.done && !cancelled)
					Thread.Sleep(1);

			}
		}

		public void Dispose()
		{
			texture?.Dispose();
		}
	}
}
