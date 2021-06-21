using CsvHelper;
using CsvHelper.Configuration;
using CsvHelper.TypeConversion;
using Microsoft.Xna.Framework.Audio;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace DataManager.Assets.Raw
{
	public class AudioClip
	{
		public readonly string FilePath;
		public readonly string RelativePath;
		public string Key => RelativePath;

		private SoundEffectInstance _effect;

		public SoundEffectInstance Sound => _effect ?? (_effect = SoundEffect.FromFile(FilePath).CreateInstance());

		public bool IsPlaying => _effect?.State == SoundState.Playing;

		public void Play()
		{
			if (Sound != null)
				Sound.Play();
		}

		public void Stop()
		{
			if (Sound != null)
				Sound.Stop();
		}

		public AudioClip(string audioPath)
		{
			FilePath = audioPath;
			RelativePath = audioPath.Substring(AssetManager.AudioDir.Length);
			RelativePath = RelativePath.Substring(0, RelativePath.Length - 4);
		}
	}

	public struct AudioClipRef
	{
		public readonly string Id;
		public readonly AudioClip Clip;

		public static readonly AudioClipRef None = new AudioClipRef(null);

		public AudioClipRef(AudioClip clip)
		{
			Clip = clip;
			Id = clip?.Key ?? string.Empty;
		}
		public AudioClipRef(string id, AudioClip clip)
		{
			Clip = clip;
			Id = id;
		}

		public override string ToString()
		{
			return Id?.ToString() ?? string.Empty;
		}

		public class CsvConverter : DefaultTypeConverter
		{

			public static AudioClipRef FromString(string text)
			{
				if (string.IsNullOrEmpty(text))
					return AudioClipRef.None;

				AudioClip clip = AppGame.AssetManager.AudioClips.FirstOrDefault(c => c.Key == text);

				return new AudioClipRef(text, clip);
			}
			public override object ConvertFromString(string text, IReaderRow row, MemberMapData memberMapData)
			{
				return FromString(text);
			}
			public override string ConvertToString(object value, IWriterRow row, MemberMapData memberMapData)
			{
				if (value == null)
					return string.Empty;

				return ((AudioClipRef)value).Id.ToString();
			}
		}
	}
}
