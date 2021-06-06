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
		private List<string> imageList = new List<string>();
		[JsonProperty]
		public SpriteAtlasPackStrategy PackStrategy;

		[JsonIgnore]
		public List<ImageList> Assets = new List<ImageList>();
		[JsonIgnore]
		public readonly int Id = -1;
		[JsonIgnore]
		public float Used { get; private set; }

		static int id = 0;

		public SpriteAtlasEntry()
		{
			Id = ++id;
		}

		public void Init()
		{
			Assets.Clear();

			foreach (var l in imageList)
			{
				var item = AppGame.AssetManager.ImageLists.FirstOrDefault(a => a.RelativePath == l);
				if (item == null || Assets.Contains(item))
					continue;

				Assets.Add(item);
			}

			Assets = Assets.OrderBy(a => a.RelativePath).ToList();
			imageList.Sort();

			RecalculateUsed();
		}

		public void SetAssets(IEnumerable<ImageList> assets)
		{
			imageList.Clear();
			Assets.Clear();


			Assets.AddRange(assets.Distinct());
			Assets = Assets.OrderBy(a => a.RelativePath).ToList();

			imageList.AddRange(Assets.Select(s => s.RelativePath));

			imageList.Sort();

			RecalculateUsed();
		}

		public void RemoveAsset(ImageList asset)
		{
			Assets.Remove(asset);
			imageList.Remove(asset.RelativePath);
			RecalculateUsed();
		}

		public static float CalculateUsage(ImageList asset)
		{
			int size = 1024 * 1024;
			int used = asset.TakenSpace;

			return (float)used / (float)size;
		}

		private void RecalculateUsed()
		{
			int size = 1024 * 1024;
			int used = Assets.Sum(a => a.TakenSpace);

			Used = (float)used / (float)size;
		}
	}
}
