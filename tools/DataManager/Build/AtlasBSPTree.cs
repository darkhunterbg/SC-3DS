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
}
