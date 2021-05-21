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
		public List<string> Paths = new List<string>();
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

			T3SFolders("unit/terran/marine", "unit/terran/tmadeath");
			T3SFolders("unit/terran/tmashad");
			T3SFolders("unit/terran/control","unit/terran/controlt");
			T3SFolders("unit/terran/tccshad");

			T3SFolders("unit/terran/scv","unit/thingy/tscglow");
			T3SFolders("unit/thingy/tbangs");
			T3SFolders("unit/thingy/tbangl");
			T3SFolders("unit/thingy/tbangx");
			GeneraSourceCode();
		}

		static void GeneraSourceCode()
		{
			using (StreamWriter s = new StreamWriter("../../src/game/Data/Generated.h"))
			{

				s.WriteLine(Template.HeaderStart);

				foreach (var e in generated)
				{
					s.WriteLine($"\tstatic SpriteFrameAtlas* {e.Name};");
				}


				s.WriteLine();

				foreach (var e in generated)
				{
					s.WriteLine($"\tstatic const SpriteFrameAtlas* Load_{e.Name}();");
				}

				s.WriteLine("};");
			}

			using (StreamWriter s = new StreamWriter("../../src/game/Data/Generated.cpp"))
			{
				s.WriteLine(Template.SourceStart);
				foreach (var e in generated)
				{
					s.WriteLine($"SpriteFrameAtlas* SpriteDatabase::{e.Name};");
				}
				s.WriteLine();

				foreach (var e in generated)
				{
					string[] info = File.ReadAllLines(Path.Combine(e.Paths[0], "info.txt")); ;
					var spl = info[0].Split(' ');
					int width = int.Parse(spl[0]);
					int height = int.Parse(spl[1]);

					s.WriteLine($"const SpriteFrameAtlas* SpriteDatabase::Load_{e.Name}() {{");
					s.WriteLine($"\tconst SpriteAtlas* atlas = Game::AssetLoader.LoadAtlas(\"{e.Name}.t3x\");");
					s.WriteLine($"\tauto* a = {e.Name} = new SpriteFrameAtlas(atlas);");
					s.WriteLine($"\ta->FrameSize = Vector2Int({width}, {height});");

					s.WriteLine();

					int x = 0;

					foreach (var path in e.Paths)
					{
						info = File.ReadAllLines(Path.Combine(path, "info.txt")); ;
						spl = info[0].Split(' ');

					
						foreach (string i in info.Skip(1))
						{
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

					}

					s.WriteLine();

					s.WriteLine("\treturn a;");

					s.WriteLine("}");

					s.WriteLine();

				}
			}

		}

		static void T3SFolders(params string[] folders)
		{

			string name = folders[0].Replace('/', '_');


			string root = Path.GetFullPath("../../data_out");

			string header = $"--atlas -f {format} -z {compression} -q high";

			string fileName = Path.GetFullPath($"../../gfx/{name}.t3s");

			List<string> paths = new List<string>();

			using (StreamWriter s = new StreamWriter(fileName))
			{
				s.WriteLine(header);

				foreach (var src in folders)
				{
					Console.WriteLine(src);

					string path = Path.GetFullPath($"../../data_out/{src}");
					paths.Add(path);
					foreach (var f in Directory.GetFiles(path, "*.png", SearchOption.AllDirectories))
					{
						s.WriteLine("..\\data_out\\" + f.Substring(root.Length + 1));
					}
				}
			}

			generated.Add(new GeneratedInfo()
			{
				Name = name,
				Centered = false,
				Paths = paths,
			});
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

";

		public static string SourceStart =
@"#include ""Generated.h""
#include ""../Game.h""

";

	}
}
