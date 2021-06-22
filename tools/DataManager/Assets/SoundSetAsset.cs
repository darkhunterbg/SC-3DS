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
	[BinaryData(DataItemType.SoundSets)]
	public class SoundSetAsset : Asset, ITreeViewItem, IInteractableTreeViewItem
	{
		public override string AssetName => Path;

		[Index(0)]
		public string Path { get; set; }

		[Ignore]
		[Binary(BinaryType.UInt, 2)]
		public int _ClipStart { get; set; }
		[Ignore]
		[Binary(BinaryType.UInt, 1)]
		public int _ClipCount { get; set; }

		[DefaultEditor]
		[Optional]
		[Binary(BinaryType.UInt, 1)]
		public bool Randomize { get; set; }


		[ListEditor]
		public List<AudioClipRef> Clips { get; set; } = new List<AudioClipRef>();

		[Optional, Name("Clips")]
		public string _Clips { get; set; }

		public override Asset Clone()
		{
			SoundSetAsset clone = (SoundSetAsset)base.Clone();
			clone.Clips = clone.Clips.ToList();

			return clone;
		}

		public override void OnBeforeSerizalize()
		{
			_Clips = string.Join(',', Clips.Select(s => s.Id ?? string.Empty));
		}

		public override void OnAfterDeserialize()
		{
			Clips.Clear();
			if (string.IsNullOrEmpty(_Clips))
			{
				return;
			}
			var split = _Clips.Split(',');
			foreach (var s in split)
			{
				Clips.Add(AudioClipRef.CsvConverter.FromString(s));
			}
		}

		int index = -1;

		public override string ActionButtonText => Clips.Any(c => c.Clip != null) ?
			"Play" : null;

		public override void Activate()
		{
			if (Clips.Count == 0)
				return;

			foreach (var c in Clips)
				c.Clip?.Stop();

			if (Randomize)
			{
				index = AppGame.Rand.Next(Clips.Count);
			}
			else
			{
				++index;
				index %= Clips.Count;
			}

			Clips[index].Clip?.Play();
		}
	}
}
