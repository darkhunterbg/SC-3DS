using System;
using System.Collections.Generic;
using System.IO;
using System.Linq;

namespace AssetBatcher
{
	class GeneratedInfo
	{
		public string Name;
		public bool Centered;
		public string Path;
	}

	class Program
	{
		static string format = "auto-etc1";// "rgba8888";
		static string compression = "auto";

		static List<GeneratedInfo> generated = new List<GeneratedInfo>();

		static void Main(string[] args)
		{
			string rootDir = Path.GetFullPath("../..");
			string path = Path.Combine(rootDir, "data_out", "unit", "terran");
			var dir = Path.Combine(rootDir, "gfx");

			T3SFolder("unit/terran/marine");
			T3SFolder("unit/terran/tmashad");

			T3SFolder("unit/terran/scv");
			GeneraSourceCode();
		}

		static void GeneraSourceCode()
		{
			using (StreamWriter s = new StreamWriter("../../src/game/Data/Generated.h")) {

				s.WriteLine(Template.HeaderStart);

				foreach (var e in generated) {
					s.WriteLine($"\tstatic SpriteFrameAtlas* {e.Name};");
				}


				s.WriteLine();

				foreach (var e in generated) {
					s.WriteLine($"\tstatic const SpriteFrameAtlas* Load_{e.Name}();");
				}

				s.WriteLine("};");
			}

			using (StreamWriter s = new StreamWriter("../../src/game/Data/Generated.cpp")) {
				s.WriteLine(Template.SourceStart);
				foreach (var e in generated) {
					s.WriteLine($"SpriteFrameAtlas* SpriteDatabase::{e.Name};");
				}
				s.WriteLine();

				foreach (var e in generated) {
					string[] info = File.ReadAllLines(Path.Combine(e.Path, "info.txt")); ;
					var spl = info[0].Split(' ');
					int width = int.Parse(spl[0]);
					int height = int.Parse(spl[1]);



					s.WriteLine($"const SpriteFrameAtlas* SpriteDatabase::Load_{e.Name}() {{");
					s.WriteLine($"\tconst SpriteAtlas* atlas = Platform::LoadAtlas(\"{e.Name}.t3x\");");
					s.WriteLine($"\tauto* a = {e.Name} = new SpriteFrameAtlas(atlas);");
					s.WriteLine($"\tUnits.push_back(a);");
					s.WriteLine($"\ta->FrameSize = Vector2Int({width}, {height});");

					s.WriteLine();

					int x = 0;
					foreach (string i in info.Skip(1)) {
						var split = i.Split(' ');
						int w = int.Parse(split[1]);
						if (e.Centered)
							w -= width / 2;
						int h = int.Parse(split[2]);
						if (e.Centered)
							h -= height / 2;

						s.WriteLine($"\ta->SetOffset({x}, Vector2Int({w},{h}));");

						++x;
					}

					s.WriteLine();

					s.WriteLine("\treturn a;");

					s.WriteLine("}");

					s.WriteLine();
				}
			}

		}

		static void T3SFolder(string folder, bool centerCoord = false)
		{
			string name = folder.Replace('/', '_');

			string path = Path.GetFullPath($"../../data_out/{folder}");
			string root = Path.GetFullPath("../../data_out");

			string header = $"--atlas -f {format} -z {compression} -q high";

			string fileName = Path.GetFullPath($"../../gfx/{name}.t3s");

			using (StreamWriter s = new StreamWriter(fileName)) {
				s.WriteLine(header);
				foreach (var f in Directory.GetFiles(path, "*.png", SearchOption.AllDirectories)) {
					s.WriteLine("..\\data_out\\" + f.Substring(root.Length + 1));
				}
			}

			generated.Add(new GeneratedInfo()
			{
				Name = name,
				Centered = centerCoord,
				Path = path,
			});
		}

		static void GenerateT3S(string dir)
		{
			foreach (var f in Directory.GetFiles(dir, "*.t3s", SearchOption.TopDirectoryOnly))
				File.Delete(f);

			var files = Directory.GetFiles(dir, "*.png", SearchOption.AllDirectories);
			var grouped = files.GroupBy(f => Path.GetDirectoryName(f)).ToDictionary(g => g.Key, g => g.ToList());

			string header = $"--atlas -f {format} -z {compression} -q high";

			foreach (var g in grouped) {
				string filePath = Path.GetFileNameWithoutExtension(g.Key);
				if (g.Key != dir)
					filePath = g.Key.Substring(dir.Length + 1);
				var fileName = filePath.Replace('\\', '_');
				string n = Path.GetFileNameWithoutExtension(g.Value[0]);
				if (g.Value.Count == 1 && !int.TryParse(n, out _)) {
					fileName = filePath.Replace('\\', '_') + "_" + n;
				}
				fileName = Path.Combine(dir, fileName) + ".t3s";

				using (StreamWriter s = new StreamWriter(fileName)) {
					string h = $"{header}";
					s.WriteLine(header);
					foreach (var p in g.Value) {
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
