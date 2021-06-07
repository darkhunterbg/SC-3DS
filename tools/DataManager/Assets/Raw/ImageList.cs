
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
		public string InfoFilePath { get; private set; }
		public string RelativePath { get; private set; }
		public string Key => RelativePath;
		public string Name { get; private set; }
		public string Dir { get; private set; }

		public Vector2 FrameSize { get; private set; }

		public List<ImageFrame> Frames { get; private set; } = new List<ImageFrame>();

		public int TakenSpace => Frames.Sum(f => f.TakenSpace);


		public int UnitColorOffset { get; private set; }

		public ImageList(string infoFile)
		{
			Dir = Path.GetDirectoryName(infoFile);
			RelativePath = Dir.Substring(AssetManager.AssetsDir.Length);
			InfoFilePath = infoFile.Substring(AssetManager.AssetsDir.Length);
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

				Frames.Add(new ImageFrame(this, Frames.Count)
				{
					fileName = name,
					rect = rect,
				});
			}

			UnitColorOffset = Frames.IndexOf(f => f.fileName.StartsWith("cm_"));
			if (UnitColorOffset < 0)
				UnitColorOffset = 0;
		}

		public int GetUnitColorFrameIndex(int frame)
		{
			return frame + UnitColorOffset;
		}

		public string GetFrameFilePath(int i)
		{
			return $"{AssetManager.AssetsDir}{RelativePath}\\{Frames[i].fileName}";
		}
	}
}
