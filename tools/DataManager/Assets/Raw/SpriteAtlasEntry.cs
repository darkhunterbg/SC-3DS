using Newtonsoft.Json;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace DataManager.Assets.Raw
{
	public enum SpriteAtlasPackStrategy
	{
		Group,
		Tight,
	}

	public class SpriteAtlasEntry
	{
		[JsonProperty]
		public string OutputName = string.Empty;
		[JsonProperty(PropertyName = "ImageList")]
		public List<string> Directories = new List<string>();
		[JsonProperty]
		public SpriteAtlasPackStrategy PackStrategy;

		[JsonIgnore]
		public List<ImageList> Assets { get; private set; } = new List<ImageList>();

		[JsonIgnore]
		public readonly int Id = -1;

		static int id = 0;

		[JsonIgnore]
		public float Usage { get; set; }

		public SpriteAtlasEntry()
		{
			Id = ++id;
		}

		public void ReloadAssets()
		{
			Assets.Clear();
			Usage = 0;

			foreach (var path in Directories)
			{
				Assets.AddRange(Util.TextFilter(AppGame.AssetManager.ImageLists, path, f => f.Key, false));
			}

			Assets = Assets.Distinct().OrderBy(a => a.Key).ToList();

			Usage = Assets.Sum(s => s.TakenSpace) / (float)(1024 * 1024);
		}

	}
}
