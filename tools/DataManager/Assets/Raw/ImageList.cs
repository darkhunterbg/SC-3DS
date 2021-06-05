﻿
using System;
using System.Collections.Generic;
using System.IO;
using System.Linq;
using System.Numerics;
using System.Text;
using System.Threading.Tasks;

namespace DataManager.Assets
{
	public class ImageList
	{
		public class FrameData
		{
			public string fileName;
			public Microsoft.Xna.Framework.Rectangle rect;
			public int index;
		}

		public string InfoFilePath { get; private set; }
		public string RelativePath { get; private set; }
		public string Name { get; private set; }
		public string Dir { get; private set; }

		public Vector2 FrameSize { get; private set; }

		public List<FrameData> Frames { get; private set; } = new List<FrameData>();

		public int TakenSpace => Frames.Sum(f => f.rect.Width * f.rect.Height + 2);

		public ImageList(string infoFile)
		{
			Dir = Path.GetDirectoryName(infoFile);
			RelativePath = Dir.Substring(AssetManager.ConvertedAssetDir.Length);
			InfoFilePath = infoFile.Substring(AssetManager.ConvertedAssetDir.Length);
			Name = Path.GetFileNameWithoutExtension(Dir);

			string[] info = File.ReadAllLines(infoFile);
			var spl = info[0].Split(' ');
			FrameSize = new Vector2(int.Parse(spl[0]), int.Parse(spl[1]));

			foreach (var e in info.Skip(1))
			{
				spl = e.Split(' ');

				Microsoft.Xna.Framework.Rectangle rect = new Microsoft.Xna.Framework.Rectangle();
				rect.X = int.Parse(spl[1]);
				rect.Y = int.Parse(spl[2]);
				rect.Width = int.Parse(spl[3]);
				rect.Height = int.Parse(spl[4]);

				string name = spl[0];

				Frames.Add(new FrameData()
				{
					fileName = name,
					rect = rect,
					index = Frames.Count
				});
			}
		}

		public string GetFrameFilePath(int i)
		{
			return $"{AssetManager.ConvertedAssetDir}{RelativePath}\\{Frames[i].fileName}";
		}
	}
}
