using System;
using System.IO;
using System.Linq;

namespace AssetBatcher
{
	class Program
	{
		static string format = "auto-etc1";// "rgba8888";
		static string compression = "auto";

		static void Main(string[] args)
		{
			string rootDir = Path.GetFullPath("../..");
			string path = Path.Combine(rootDir, "data_out", "unit", "terran");
			var dir = Path.Combine(rootDir, "gfx");

			T3SFolder("unit/terran/marine", true);
		}

		static void T3SFolder(string folder, bool centerCoord = false)
		{
			string name = folder.Replace('/', '_');

			string path = Path.GetFullPath($"../../data_out/{folder}");
			string root = Path.GetFullPath("../../data_out");

			string header = $"--atlas -f {format} -z {compression} -q high";

			string fileName = Path.GetFullPath($"../../gfx/{name}.t3s");

			using (StreamWriter s = new StreamWriter(fileName))
			{
				s.WriteLine(header);
				foreach (var f in Directory.GetFiles(path, "*.png", SearchOption.AllDirectories))
				{
					s.WriteLine("..\\data_out\\" + f.Substring(root.Length + 1));
				}
			}

			string[] info = File.ReadAllLines(Path.Combine(path, "info.txt"));
			var spl = info[0].Split(' ');
			int width = int.Parse(spl[0]);
			int height = int.Parse(spl[1]);

			using (StreamWriter s = new StreamWriter("../../src/game/Data/Generated.h"))
			{
				s.WriteLine(Template.HeaderStart);

				s.WriteLine($"\tstatic SpriteFrameAtlas* {name};");
				s.WriteLine($"\tstatic const SpriteFrameAtlas* Load_{name}();");

				s.WriteLine();
				s.WriteLine("};");
			}
			using (StreamWriter s = new StreamWriter("../../src/game/Data/Generated.cpp"))
			{
				s.WriteLine(Template.SourceStart);

				s.WriteLine($"SpriteFrameAtlas* SpriteDatabase::{name};");
				s.WriteLine($"const SpriteFrameAtlas* SpriteDatabase::Load_{name}() {{");
				s.WriteLine($"\tconst SpriteAtlas* atlas = Platform::LoadAtlas(\"{ name}.t3x\");");
				s.WriteLine($"\tauto* a = {name} = new SpriteFrameAtlas(atlas);");
				s.WriteLine($"\tUnits.push_back(a);");
				s.WriteLine($"\ta->FrameSize = Vector2Int({width}, {height});");

				s.WriteLine();

				int x = 0;
				foreach (string i in info.Skip(1))
				{
					var split = i.Split(' ');
					int w = int.Parse(split[1]);
					if (centerCoord)
						w -= width / 2;
					int h = int.Parse(split[2]);
					if (centerCoord)
						h -= height / 2;

					s.WriteLine($"\ta->SetOffset({x}, Vector2Int({w},{h}));");

					++x;
				}

				s.WriteLine();

				s.WriteLine("\treturn a;");

				s.WriteLine("}");
			}
		}

		static void GenerateT3S(string dir)
		{

			foreach (var f in Directory.GetFiles(dir, "*.t3s", SearchOption.TopDirectoryOnly))
				File.Delete(f);

			var files = Directory.GetFiles(dir, "*.png", SearchOption.AllDirectories);
			var grouped = files.GroupBy(f => Path.GetDirectoryName(f)).ToDictionary(g => g.Key, g => g.ToList());

			string header = $"--atlas -f {format} -z {compression} -q high";

			foreach (var g in grouped)
			{
				string filePath = Path.GetFileNameWithoutExtension(g.Key);
				if (g.Key != dir)
					filePath = g.Key.Substring(dir.Length + 1);
				var fileName = filePath.Replace('\\', '_');
				string n = Path.GetFileNameWithoutExtension(g.Value[0]);
				if (g.Value.Count == 1 && !int.TryParse(n, out _))
				{
					fileName = filePath.Replace('\\', '_') + "_" + n;
				}
				fileName = Path.Combine(dir, fileName) + ".t3s";

				using (StreamWriter s = new StreamWriter(fileName))
				{
					string h = $"{header}";
					s.WriteLine(header);
					foreach (var p in g.Value)
					{
						var f = p.Substring(dir.Length + 1);
						s.WriteLine(f);
					}
				}
			}
		}
	}

	class Template
	{
		public static string HeaderStart = @"#pragma once

#include ""../Assets.h""
#include <vector>

class SpriteDatabase{

	private:
	SpriteDatabase() = delete;
	~SpriteDatabase() = delete;

public:

	static std::vector<const SpriteFrameAtlas*> Units;

";

		public static string SourceStart =
@"#include ""Generated.h""
#include ""../Platform.h""

std::vector<const SpriteFrameAtlas*> SpriteDatabase::Units;";


	}
}
