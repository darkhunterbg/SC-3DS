using CsvHelper.Configuration.Attributes;
using DataManager.Assets.Raw;
using DataManager.Build;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace DataManager.Assets
{
	[BinaryData(DataItemType.UnitPortrait)]
	public class UnitPortraitAsset : Asset, ITreeViewItem
	{
		public override string AssetName => Path;

		public override GuiTexture Preview => GetClips().FirstOrDefault()?.GetPreview();

		[Index(0)]
		[Binary(BinaryType.String, 32)]
		public string Path { get; set; }

		[Ignore]
		[Binary(BinaryType.UInt, 2)]
		public int _IdleClipStart { get; set; }
		[Ignore]
		[Binary(BinaryType.UInt, 1)]
		public int _IdleClipCount { get; set; }

		[Ignore]
		[Binary(BinaryType.UInt, 2)]
		public int _ActivatedClipStart { get; set; }
		[Ignore]
		[Binary(BinaryType.UInt, 1)]
		public int _ActivatedClipCount { get; set; }

		[ListEditor]
		public List<VideoClipRef> IdleClips { get; set; } = new List<VideoClipRef>();

		[ListEditor]
		public List<VideoClipRef> ActivatedClips { get; set; } = new List<VideoClipRef>();

		public override bool HasTooltip => true;

		public override void DrawTooltip()
		{
			GetClips().FirstOrDefault()?.GUIPreview();
		}

		private IEnumerable<VideoClip> GetClips()
		{
			foreach(var c in IdleClips) yield return c.Clip;
			foreach (var c in ActivatedClips) yield return c.Clip;
		}

		[Optional, Name("IdleClips")]
		public string _IdleClips { get; set; }

		[Optional, Name("ActivatedClips")]
		public string _ActivatedClips { get; set; }

		public override Asset Clone()
		{
			UnitPortraitAsset clone = (UnitPortraitAsset)base.Clone();
			clone.IdleClips = clone.IdleClips.ToList();
			clone.ActivatedClips = clone.ActivatedClips.ToList();

			return clone;
		}

		public override void OnBeforeSerizalize()
		{
			_IdleClips = string.Join(',', IdleClips.Select(s => s.Id ?? string.Empty));
			_ActivatedClips = string.Join(',', ActivatedClips.Select(s => s.Id ?? string.Empty));
		}

		public override void OnAfterDeserialize()
		{
			IdleClips.Clear();
			ActivatedClips.Clear();
			if (!string.IsNullOrEmpty(_IdleClips)) {
				var split = _IdleClips.Split(',');
				foreach (var s in split) {
					IdleClips.Add(VideoClipRef.CsvConverter.FromString(s));
				}
			}

			if (!string.IsNullOrEmpty(_ActivatedClips)) {
				var split = _ActivatedClips.Split(',');
				foreach (var s in split) {
					ActivatedClips.Add(VideoClipRef.CsvConverter.FromString(s));
				}
			}

		}
	}
}
