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
using DataManager.Gameplay;

namespace DataManager.Build
{
	public class BuildGenerator : IDisposable
	{
		List<string> additionalBuildFiles = new List<string>()
			{
				 "tileset\\tile.png" ,
				"font.ttf", "mm-font.ttf" ,
				"Smk\\Blizzard.smk",
				 "glue\\mainmenu\\exit.smk",  "glue\\mainmenu\\exiton.smk",
				  "glue\\mainmenu\\single.smk",  "glue\\mainmenu\\singleon.smk",
				   "glue\\mainmenu\\multi.smk",  "glue\\mainmenu\\multion.smk",
					 "glue\\mainmenu\\editor.smk",  "glue\\mainmenu\\editoron.smk",
			};


		IEnumerable<PackEntry> GetAdditionalPackVideo()
		{
			return additionalBuildFiles.Where(s => s.EndsWith(".smk"))
				.Select(s => new PackEntry()
				{
					RelativePath = s.Substring(0, s.Length - 4),
					FilePath = AssetManager.StarcraftAssetDir + s
				});
		}

		IEnumerable<PackEntry> GetAdditionalPackAudio()
		{
			return additionalBuildFiles.Where(s => s.EndsWith(".wav"))
				.Select(s => new PackEntry()
				{
					RelativePath = s.Substring(0, s.Length - 4),
					FilePath = AssetManager.StarcraftAssetDir + s
				});
		}

		IEnumerable<PackEntry> GetAdditionalPackOther()
		{
			return additionalBuildFiles.Where(s => !s.EndsWith(".wav") && !s.EndsWith("smk"))
				.Select(s => new PackEntry()
				{
					RelativePath = s,
					FilePath = AssetManager.AssetsDir + s
				});
		}


		public static BuildGenerator Instance { get; private set; }

		private List<SpriteAtlasEntry> atlases = null;

		private volatile bool cancelled = false;

		public bool DrawAtlasOutiline { get; set; }
		public bool Build3DS { get; set; }
		public bool GenerateCSVImageData { get; set; }

		int totalJobs = 0;
		int currentJob = 0;

		private AsyncOperation op;

		public BuildGenerator()
		{
			Instance = this;
		}

		private void DisplayItem(string text)
		{
			op.ItemName = $"{text} {currentJob}/{totalJobs}";
		}
		private void Progress(int current, int max)
		{
			op.Progress = (float)(current) / (float)max;
		}

		public List<SpriteAtlas> GeneratedAtlases { get; private set; } = new List<SpriteAtlas>();
		public List<SpriteSubAtlas> GeneratedSubAtlases { get; private set; } = new List<SpriteSubAtlas>();
		public List<ImageFrameAtlasData> GeneratedFrames { get; private set; } = new List<ImageFrameAtlasData>();
		public List<ImageListAtlasData> GeneratedImages { get; private set; } = new List<ImageListAtlasData>();

		private SpriteBatch spriteBatch = new SpriteBatch(AppGame.Device);

		public AsyncOperation BuildAtlases(List<SpriteAtlasEntry> atlases)
		{
			this.atlases = atlases;

			totalJobs = atlases.Count;

			return new AsyncOperation(BuildAsync, OnCancelAsync);
		}

		private void BuildAsync(AsyncOperation op)
		{
			this.op = op;

			DisplayItem("Cleaning...");

			totalJobs = 1;
			currentJob = 0;

			Directory.Delete(AssetManager.CookedAssetsPCDir, true);
			Directory.CreateDirectory(AssetManager.CookedAssetsPCDir);
			if (Build3DS) {
				Directory.Delete(AssetManager.Cooked3DSAssetsDir, true);
				Directory.CreateDirectory(AssetManager.Cooked3DSAssetsDir);

				string ciaDst = Path.Combine(Path.GetFullPath("..\\.."), "SC.cia");
				if (File.Exists(ciaDst))
					File.Delete(ciaDst);
			}


			totalJobs = atlases.Count;
			foreach (var atlas in atlases) {
				++currentJob;

				DisplayItem(atlas.OutputName);

				List<AtlasBSPTree> subAtlases = GenerateAtlasTree(atlas);

				if (cancelled)
					return;

				List<AtlasBSPTree> scaledSubAtlases = SimplifyAtlasTrees(subAtlases);

				var crt = AppGame.RunCoroutine(SaveAtlasTextureCrt(scaledSubAtlases, atlas));

				while (!crt.done && !cancelled)
					Thread.Sleep(1);

			}


			if (cancelled)
				return;

			GenerateAtlasData();

			if (cancelled)
				return;

			if (Build3DS)
				BuildT3XFiles();

			if (cancelled)
				return;

			BuildData();

			if (cancelled)
				return;

			BuildAudio();

			if (cancelled)
				return;

			BuildVideo();

			if (cancelled)
				return;

			BuildOther();

			if (cancelled)
				return;

			if (Build3DS)
				BuildCIA();
		}

		private static List<AtlasBSPTree> SimplifyAtlasTrees(List<AtlasBSPTree> subAtlases)
		{
			if (subAtlases.Count == 1) {
				var f = subAtlases[0].GetFrames();
				if (f.Count == 1) {
					var r = new AtlasBSPTree(f[0].rect.Width, f[0].rect.Height);
					r.TryAdd(f[0].image);

					return new List<AtlasBSPTree>() { r };
				}
			}

			List<AtlasBSPTree> scaledSubAtlases = new List<AtlasBSPTree>();

			foreach (var subAtlas in subAtlases) {
				var tree = subAtlas;

				while (tree.TakenSpace < (tree.TotalSpace / 4) && tree.TotalSpace >= 128 * 128) {
					bool success = true;

					var test = new AtlasBSPTree(tree.Dimensions.X / 2, tree.Dimensions.Y / 2);
					var frames = tree.GetFrames();

					foreach (var f in frames) {
						if (!test.TryAdd(f.image)) {
							success = false;
							break;
						}
					}
					if (!success)
						break;

					tree = test;
				}

				scaledSubAtlases.Add(tree);
			}

			return scaledSubAtlases;
		}

		private void OnCancelAsync()
		{
			cancelled = true;
		}

		private IEnumerator SaveAtlasTextureCrt(List<AtlasBSPTree> subAtlases, SpriteAtlasEntry atlas)
		{
			SpriteAtlas generated = new SpriteAtlas(atlas.OutputName);
			GeneratedAtlases.Add(generated);

			int i = 0;

			foreach (var subAtlas in subAtlases) {
				if (cancelled)
					yield break;

				var tree = subAtlas;

				using (var texture = new RenderTarget2D(AppGame.Device, tree.Dimensions.X, tree.Dimensions.Y)) {

					texture.GraphicsDevice.SetRenderTarget(texture);
					texture.GraphicsDevice.Clear(Color.Transparent);

					var frames = tree.GetFrames();

					spriteBatch.Begin(samplerState: SamplerState.PointClamp);

					foreach (var f in frames) {
						spriteBatch.Draw(f.image.Image.Texture, f.rect.Location.ToVector2(), Color.White);
					}
					if (DrawAtlasOutiline) {
						foreach (var f in tree.GetNodes()) {
							spriteBatch.DrawRectangle(f.Region.Location.ToVector2().ToVec2(),
				new Vector2(1, f.Region.Size.Y),
				 Color.Magenta);


							spriteBatch.DrawRectangle(f.Region.Location.ToVector2().ToVec2()
								+ new Vector2(f.Region.Size.X - 1, 0),
							new Vector2(1, f.Region.Size.Y),
							 Color.Magenta);

							spriteBatch.DrawRectangle(f.Region.Location.ToVector2().ToVec2(),
								new Vector2(f.Region.Size.X, 1),
								 Color.Magenta);

							spriteBatch.DrawRectangle(f.Region.Location.ToVector2().ToVec2()
								+ new Vector2(0, f.Region.Size.Y - 1),
							new Vector2(f.Region.Size.X, 1),
							 Color.Magenta);
						}
					}

					spriteBatch.End();

					generated.AddSubAtlas(frames.Select(s =>
					new ImageFrameAtlasData(s.image, s.rect.Location.ToVector2().ToVec2()))
						.ToList());

					texture.SaveAsPng($"{AssetManager.SpriteAtlasOutDir}{generated.SubAtlases.Last().FullName}.png");

				}

				i++;
				yield return null;
			}
		}

		private ImageList FindFittingImageList(List<ImageList> assets, AtlasBSPTree tree, out AtlasBSPTree packedTree)
		{
			packedTree = null;

			foreach (var imageList in assets.Where(t => t.TakenSpace <= tree.FreeSpace)) {
				if (cancelled)
					return null;

				AtlasBSPTree test = tree.Clone();
				bool success = true;

				foreach (var frame in imageList.Frames.OrderByDescending(t => t.rect.Size.Y)
					.ThenBy(t => t.rect.Size.X)) {
					if (!test.TryAdd(frame)) {
						success = false;
						break;
					}
				}

				if (success) {
					packedTree = test;
					return imageList;
				}
			}

			return null;
		}

		private List<AtlasBSPTree> GenerateAtlasTree(SpriteAtlasEntry atlas)
		{
			var assets = atlas.GetImageListsForBuild();
			foreach (var asset in assets) {
				GeneratedImages.Add(new ImageListAtlasData(asset));
			}

			int assetTotal = assets.Count;
			int assetProgress = 1;

			AtlasBSPTree current = new AtlasBSPTree();

			List<AtlasBSPTree> subAtlases = new List<AtlasBSPTree>();


			if (atlas.PackStrategy == SpriteAtlasPackStrategy.TightPacking)
				goto TightPacking;


			while (assets.Count > 0) {
				var add = FindFittingImageList(assets, current, out var packed);
				if (cancelled)
					return null;

				if (add != null) {
					assets.Remove(add);

					current = packed;

					++assetProgress;
					Progress(assetProgress, assetTotal);
				} else {
					if (current.IsEmpty()) {
						// Cant  find anything to fit in empty, remaining should be tighlty-apcked
						break;
					}
					subAtlases.Add(current);
					current = new AtlasBSPTree();
				}
			}

			if (!current.IsEmpty()) {
				subAtlases.Add(current);
				current = new AtlasBSPTree();
			}

		TightPacking:
			foreach (var imageList in assets) {
				if (cancelled)
					return subAtlases;

				foreach (var frame in imageList.Frames.OrderByDescending(t => t.rect.Size.Y)
					.ThenBy(t => t.rect.Size.X)) {
					bool success = false;

					foreach (var sub in subAtlases.Where(t => t.FreeSpace >= frame.TakenSpace)) {
						if (sub.TryAdd(frame)) {
							success = true;
							break;
						}
					}


					if (!success) {
						var newAtlas = new AtlasBSPTree();
						newAtlas.TryAdd(frame);
						subAtlases.Add(newAtlas);
					}
				}


				++assetProgress;
				Progress(assetProgress, assetTotal);

			}

			if (!current.IsEmpty()) {
				subAtlases.Add(current);
				current = new AtlasBSPTree();
			}

			return subAtlases;
		}

		private void GenerateAtlasData()
		{
			GeneratedImages = GeneratedImages.OrderBy(t => t.Name).ToList();

			GeneratedFrames = GeneratedAtlases.SelectMany(s => s.SubAtlases).SelectMany(s => s.Images)
				.OrderBy(s => s.Frame.ImageListName).ThenBy(s => s.Frame.FrameIndex).ToList();

			GeneratedSubAtlases = GeneratedAtlases.SelectMany(s => s.SubAtlases).OrderBy(a => a.AtlasName)
		.ThenBy(a => a.AtlasIndex).ToList();

			foreach (var img in GeneratedImages) {
				img.FrameOffset = GeneratedFrames.FindIndex(i => i.Frame.ImageList == img.List);
			}

			int i = 0;
			foreach (var subAtlases in GeneratedSubAtlases) {
				foreach (var f in subAtlases.Images)
					f.AtlasId = i;

				++i;
			}

			if (GenerateCSVImageData) {
				using (var csv = new CsvWriter(new StreamWriter(AssetManager.ImagesDataPath), AssetManager.CsvConfig)) {
					csv.WriteRecords(GeneratedImages);
				}

				using (var csv = new CsvWriter(new StreamWriter(AssetManager.FramesDataPath), AssetManager.CsvConfig)) {
					csv.WriteRecords(GeneratedFrames);
				}

				using (var csv = new CsvWriter(new StreamWriter(AssetManager.SpriteAtlasDataPath), AssetManager.CsvConfig)) {
					csv.WriteRecords(GeneratedSubAtlases);
				}
			}
		}

		private void BuildT3XFiles()
		{
			var subAtlases = GeneratedAtlases.SelectMany(s => s.SubAtlases).ToList();

			Directory.CreateDirectory(AssetManager.SpriteAtlas3DSBuildDir);

			currentJob = 0;
			totalJobs = subAtlases.Count();

			Progress(currentJob, totalJobs);

			foreach (var atlas in subAtlases) {
				if (cancelled)
					return;

				currentJob++;

				DisplayItem($"{atlas.FullName}.t3x");

				string inputTexture = Path.GetFullPath($"{AssetManager.SpriteAtlasOutDir}{atlas.FullName}") + ".png";
				string outAtlas = Path.GetFullPath(Path.Combine(AssetManager.SpriteAtlas3DSBuildDir, atlas.FullName)) + ".t3x";

				string args = $" -f auto-etc1 -z auto -q high -o {outAtlas} {inputTexture}";
				var process = new ProcessStartInfo(AssetManager.tex3dsPath, args);
				process.UseShellExecute = false;
				process.CreateNoWindow = true;
				process.RedirectStandardOutput = true;
				process.RedirectStandardError = true;
				var p = new Process()
				{
					StartInfo = process
				};


				p.Start();

				p.WaitForExit();

				string error = p.StandardError.ReadToEnd();
				if (!string.IsNullOrEmpty(error))
					throw new Exception($"Failed to build atlas {atlas.FullName}: {error}");


				Progress(currentJob, totalJobs);
			}
		}

		private void BuildAudio()
		{
			DisplayItem("Audio Clips");

			var sounds = AppGame.AssetManager.GetAssets<SoundSetAsset>();

			var items = sounds.SelectMany(s => s.Clips).Where(c => !c.IsEmpty).Select(s => s.Clip).ToList();
			items.AddRange(AppGame.AssetManager.AudioClips.Where(c => c.RelativePath.Contains("sound\\misc")));
			items = items.Distinct().ToList();



			var audio = items.Select(s => new PackEntry() { FilePath = s.FilePath, RelativePath = s.RelativePath }).ToList();
			audio.AddRange(GetAdditionalPackAudio());
			audio.Distinct().ToList();
			PackAssets(audio, Path.Combine(AssetManager.CookedAssetsPCDir, "audio.pak"));

			if (Build3DS)
				PackAssets(audio, Path.Combine(AssetManager.Cooked3DSAssetsDir, "audio.pak"));
		}

		private void BuildVideo()
		{
			DisplayItem("Video Clips");

			var portraits = AppGame.AssetManager.GetAssets<UnitPortraitAsset>();

			var items = portraits.SelectMany(s => s.IdleClips).Where(c => !c.IsEmpty).Select(s => s.Clip).ToList();
			items.AddRange(portraits.SelectMany(s => s.ActivatedClips).Where(c => !c.IsEmpty).Select(s => s.Clip).ToList());
			items = items.Distinct().ToList();


			var videos = items.Select(s => new PackEntry() { FilePath = s.FilePath, RelativePath = s.RelativePath }).ToList();
			videos.AddRange(GetAdditionalPackVideo());
			videos = videos.Distinct().ToList();
			PackAssets(videos, Path.Combine(AssetManager.CookedAssetsPCDir, "video.pak"));

			if (Build3DS)
				PackAssets(videos, Path.Combine(AssetManager.Cooked3DSAssetsDir, "video.pak"));
		}

		private void BuildOther()
		{
			currentJob = 0;
			totalJobs = GetAdditionalPackOther().Count();

			Progress(currentJob, totalJobs);

			foreach (var f in GetAdditionalPackOther()) {
				DisplayItem(f.RelativePath);

				//var src = Path.Combine(AssetManager.AssetsDir, f);
				//if (!File.Exists(src)) {
				//	src = Path.Combine(AssetManager.StarcraftAssetDir, f);
				//}



				var dst = Path.Combine(AssetManager.CookedAssetsPCDir, f.RelativePath);
				Directory.CreateDirectory(Path.GetDirectoryName(dst));
				File.Copy(f.FilePath, dst, true);

				if (Build3DS) {

					if (f.RelativePath.EndsWith(".png")) {

						string inputTexture = f.FilePath;
						string outFile = Path.ChangeExtension(Path.GetFullPath(Path.Combine(AssetManager.Cooked3DSAssetsDir, f.RelativePath)), "t3x");

						Directory.CreateDirectory(Path.GetDirectoryName(outFile));

						string args = $" -f auto-etc1 -z auto -q high -o {outFile} {inputTexture}";
						var process = new ProcessStartInfo(AssetManager.tex3dsPath, args);
						process.UseShellExecute = false;
						process.CreateNoWindow = true;
						process.RedirectStandardOutput = true;
						process.RedirectStandardError = true;
						var p = new Process()
						{
							StartInfo = process
						};


						p.Start();

						p.WaitForExit();

						string error = p.StandardError.ReadToEnd();
						if (!string.IsNullOrEmpty(error))
							throw new Exception($"Failed to build atlas {f}: {error}");
					} else if (f.RelativePath.EndsWith(".ttf")) {

						string input = f.FilePath;
						string outFile = Path.ChangeExtension(Path.GetFullPath(Path.Combine(AssetManager.Cooked3DSAssetsDir, f.RelativePath)), "bcfnt");

						Directory.CreateDirectory(Path.GetDirectoryName(outFile));

						string args = $" -o {outFile} {input}";
						var process = new ProcessStartInfo(AssetManager.mkbcfntPath, args);
						process.UseShellExecute = false;
						process.CreateNoWindow = true;
						process.RedirectStandardOutput = true;
						process.RedirectStandardError = true;
						var p = new Process()
						{
							StartInfo = process
						};


						p.Start();

						p.WaitForExit();

						string error = p.StandardError.ReadToEnd();
						if (!string.IsNullOrEmpty(error))
							throw new Exception($"Failed to build font {f}: {error}");

					} else {
						string outFile = Path.Combine(AssetManager.Cooked3DSAssetsDir, f.RelativePath);
						Directory.CreateDirectory(Path.GetDirectoryName(outFile));
						File.Copy(f.RelativePath, outFile, true);
					}
				}

				++currentJob;
				Progress(currentJob, totalJobs);
			}
		}

		private void BuildData()
		{
			List<AnimClipInstructionData> instructionData = new List<AnimClipInstructionData>();

			foreach (var clip in AppGame.AssetManager.GetAssets<SpriteAnimClipAsset>()
				.OrderBy(s => s.Sprite.AssetName).ThenBy(s => s.Type)) {
				clip._InstructionStart = (uint)instructionData.Count;


				instructionData.AddRange(clip.TrueInstructions.Select(s => new AnimClipInstructionData(clip, s)));
			}

			List<AudioClip> audioClipsData = new List<AudioClip>();

			foreach (var asset in AppGame.AssetManager.GetAssets<SoundSetAsset>()) {
				asset._ClipStart = audioClipsData.Count;
				var items = asset.Clips.Where(s => !s.IsEmpty && s.Clip != null);
				audioClipsData.AddRange(items.Select(s => s.Clip));
				asset._ClipCount = items.Count();
			}


			List<VideoClip> videoClipsData = new List<VideoClip>();

			foreach (var asset in AppGame.AssetManager.GetAssets<UnitPortraitAsset>()) {
				asset._IdleClipStart = videoClipsData.Count;
				var items = asset.IdleClips.Where(s => !s.IsEmpty && s.Clip != null);
				videoClipsData.AddRange(items.Select(s => s.Clip));
				asset._IdleClipCount = items.Count();

				asset._ActivatedClipStart = videoClipsData.Count;
				items = asset.ActivatedClips.Where(s => !s.IsEmpty && s.Clip != null);
				videoClipsData.AddRange(items.Select(s => s.Clip));
				asset._ActivatedClipCount = items.Count();
			}



			List<DataItem> data = new List<DataItem>();
			data.Add(new DataItem()
			{
				Name = "Atlases",
				Data = GeneratedSubAtlases,
			});
			data.Add(new DataItem()
			{
				Name = "Images",
				Data = GeneratedImages,
			});
			data.Add(new DataItem()
			{
				Name = "Frames",
				Data = GeneratedFrames,
			});
			data.Add(new DataItem()
			{
				Name = "Animation Instructions",
				Data = instructionData,
			});
			data.Add(new DataItem()
			{
				Name = "Audio Clips",
				Data = audioClipsData.Select(s => (object)s)
			});
			data.Add(new DataItem()
			{
				Name = "Video Clips",
				Data = videoClipsData.Select(s => (object)s)
			});


			foreach (var assetDb in AppGame.AssetManager.Assets.Values) {
				assetDb.PrepareForSerialization();
				data.Add(new DataItem()
				{
					Name = assetDb.Type.Name,
					Data = assetDb.Assets
				});
			}

			data.RemoveAll(t => t.Data.Count() == 0);

			foreach (var d in data)
				d.Init();

			currentJob = 0;
			totalJobs = data.Count;

			Progress(currentJob, totalJobs);

			DataItemHeader[] headers = data.Select(s => s.GetHeader()).ToArray();


			uint offset = (uint)(headers.Length * DataItemHeader.Size);
			offset += 4;

			List<MemoryStream> binData = new List<MemoryStream>();

			foreach (var item in data) {
				headers[currentJob].Offset = offset;

				++currentJob;
				DisplayItem(item.Name);

				var stream = item.MemorySerialize();
				binData.Add(stream);

				offset += (uint)stream.Length;

				Progress(currentJob, totalJobs);
			}

			byte[] binHeaders = headers.SelectMany(s => s.Serialize()).ToArray();
			currentJob = 0;
			totalJobs = data.Count;

			string dst = Path.Combine(AssetManager.CookedAssetsPCDir, AssetManager.BuildDataFile);
			using (FileStream stream = new FileStream(dst, FileMode.Create)) {
				stream.Write(BitConverter.GetBytes((uint)headers.Length));

				DisplayItem("Data Headers");
				stream.Write(binHeaders);

				DisplayItem("Data Sections");

				foreach (var str in binData) {
					str.Position = 0;
					++currentJob;
					byte[] bin = new byte[str.Length];
					str.Read(bin, 0, (int)str.Length);
					stream.Write(bin);
					Progress(currentJob, totalJobs);
				}
			}

			if (Build3DS) {
				string outFile = Path.Combine(AssetManager.Cooked3DSAssetsDir, AssetManager.BuildDataFile);
				Directory.CreateDirectory(Path.GetDirectoryName(outFile));
				File.Copy(dst, outFile, true);
			}
		}

		private void BuildCIA()
		{
			string romFSSrc = Path.Combine(AssetManager.CookedAssetsRootDir, "romfs_3ds.bin");
			RomFSBuilder.Build(AssetManager.Cooked3DSAssetsDir, romFSSrc, op);

			if (op.Cancelled)
				return;

			op.SetProgress(1, 1);
			op.ItemName = $"Building CIA...";

			string rootDir = "..\\..";
			string ciaDst = Path.Combine(rootDir, "SC.cia");
			string rsfSrc = Path.Combine(rootDir, "RSF");
			string elfSrc = Path.Combine(rootDir, "SC.elf");
			string smdhSrc = Path.Combine(rootDir, "SC.smdh");
			string bannerSrc = Path.Combine(rootDir, "banner.bnr");

			if (!File.Exists(elfSrc))
				throw new Exception($"ELF not found: '{elfSrc}'. Build 3ds C++ project first!");



			string args = $"-f cia -o {ciaDst} -target t -rsf {rsfSrc} -elf {elfSrc} -icon {smdhSrc} -banner {bannerSrc} -romfs {romFSSrc}";
			var process = new ProcessStartInfo(AssetManager.makeromPath, args);
			process.UseShellExecute = false;
			process.CreateNoWindow = true;
			process.RedirectStandardOutput = true;
			process.RedirectStandardError = true;
			var p = new Process()
			{
				StartInfo = process
			};


			p.Start();

			p.WaitForExit();

			string error = p.StandardError.ReadToEnd();
			if (!string.IsNullOrEmpty(error))
				throw new Exception($"Failed to build CIA: {error}");
		}

		public void Dispose()
		{
			spriteBatch.Dispose();
		}

		class PackEntry
		{
			public string FilePath;
			public string RelativePath;
			public uint Offset;
			public uint Size;
		}


		void PackAssets(List<PackEntry> files, string dst)
		{
			currentJob = 0;
			totalJobs = files.Count();

			Progress(currentJob, totalJobs);

			DisplayItem($"Packing {dst}");


			using (MemoryStream header = new MemoryStream()) {

				header.Write(BinaryAttribute.Serialize(files.Count(), typeof(uint)));

				foreach (var file in files) {


					byte[] path = BinaryAttribute.Serialize(file.RelativePath, BinaryType.String, 64);
					header.Write(path);

					// File offset 
					header.Write(BinaryAttribute.Serialize(0, typeof(uint)));
					// File size
					file.Size = (uint)new FileInfo(file.FilePath).Length;
					header.Write(BinaryAttribute.Serialize(file.Size, typeof(uint)));
				}

				int fileIndex = 0;

				if (!Directory.Exists(Path.GetDirectoryName(dst)))
					Directory.CreateDirectory(Path.GetDirectoryName(dst));

				uint fileSize = 0;
				using (FileStream fs = new FileStream(dst, FileMode.CreateNew)) {

					header.Position = 0;
					fs.Write(header.ToArray());

					foreach (var file in files) {
						++currentJob;

						DisplayItem(file.RelativePath);

						file.Offset = (uint)fs.Position;
						header.Position = 4 + fileIndex * (64 + 4 + 4) + 64;
						header.Write(BinaryAttribute.Serialize(file.Offset, typeof(uint)));

						fs.Write(File.ReadAllBytes(file.FilePath));

						++fileIndex;

						Progress(currentJob, totalJobs);
					}

					// Override updated header

					fs.Position = 0;
					header.Position = 0;
					fs.Write(header.ToArray());
					fileSize = (uint)fs.Length;
				}


				if (fileSize != (uint)header.Length + files.Sum(s => s.Size))
					throw new Exception("Pak file size mismatch!");

				for (int i = 0; i < files.Count - 1; ++i) {
					if (files[i].Offset + files[i].Size != files[i + 1].Offset)
						throw new Exception("Pak files overlapping!");
				}
			}
		}
	}
}
