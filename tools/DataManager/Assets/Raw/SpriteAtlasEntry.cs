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
		GroupPacking,
		TightPacking,
	}

	public enum SpriteAtlasFilterStrategy
	{
		OnlyReferenced,
		All
	}

	public class SpriteAtlasEntry
	{
		[JsonProperty]
		public string OutputName = string.Empty;
		[JsonProperty(PropertyName = "ImageList")]
		public List<string> Directories = new List<string>();
		[JsonProperty]
		public SpriteAtlasPackStrategy PackStrategy;
		[JsonProperty]
		public SpriteAtlasFilterStrategy FilterStrategy;

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


		public List<ImageList> GetImageListsForBuild()
		{
			List<ImageList> lists = new List<ImageList>();

			switch (FilterStrategy)
			{
				case SpriteAtlasFilterStrategy.All:
					{
						lists.AddRange(Assets);
						break;
					}
				case SpriteAtlasFilterStrategy.OnlyReferenced:
					{
						foreach (var assetDb in AppGame.AssetManager.Assets.Values)
						{
							var props = assetDb.Type.GetProperties().Where(p => p.PropertyType == typeof(ImageListRef)).ToList();

							foreach (var asset in assetDb.Assets)
							{
								foreach (var prop in props)
								{
									var value = prop.GetValue(asset);
									if (value == null)
										continue;

									var img = ((ImageListRef)value).Image;
									if (lists.Contains(img))
										continue;

									if (Assets.Contains(img))
										lists.Add(img);
								}
							}
						}

						break;
					}
			}

		

			lists = lists
			   .OrderByDescending(t => t.FrameSize.Y)
			   .ThenBy(t => t.FrameSize.X)
			   .ThenBy(t => t.TakenSpace).ToList();

			return lists;

		}
	}
}
