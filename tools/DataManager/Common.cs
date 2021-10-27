using CsvHelper;
using CsvHelper.Configuration;
using CsvHelper.TypeConversion;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Numerics;
using System.Runtime.ExceptionServices;
using System.Text;
using System.Text.RegularExpressions;
using System.Threading.Tasks;

namespace DataManager
{
	public enum CustomEnumType
	{
		SelectionTypes
	}

	public static class CsvConverters
	{

		public class Vector2Coverter : DefaultTypeConverter
		{
			public override object ConvertFromString(string text, IReaderRow row, MemberMapData memberMapData)
			{
				var s = text.Split(",");
				if (s.Length < 2)
					return Vector2.Zero;

				if(!int.TryParse(s[0],out int x))
					return Vector2.Zero;

				if (!int.TryParse(s[1], out int y))
					return Vector2.Zero;

				return new Vector2(x, y);
			}
			public override string ConvertToString(object value, IWriterRow row, MemberMapData memberMapData)
			{
				if (value == null)
					return "0,0";
				Vector2 v = (Vector2)(value);
				return $"{(int)v.X},{(int)v.Y}";
			}
		}
	}

	public static class EnumCacheValues
	{
		private static Dictionary<Type, string[]> cache = new Dictionary<Type, string[]>();
		public static string[] GetValues(Type type)
		{
			if (!cache.TryGetValue(type, out var items))
			{
				items = Enum.GetNames(type);
				cache[type] = items.Select(t => Regex.Replace(t, "(\\B[A-Z])", " $1")).ToArray();
			}

			return items;
		}
		public static string[] GetValues<TEnum>() where TEnum : struct, Enum
		{
			if (!cache.TryGetValue(typeof(TEnum), out var items))
			{
				items = Enum.GetNames<TEnum>();
				cache[typeof(TEnum)] = items.Select(t => Regex.Replace(t, "(\\B[A-Z])", " $1")).ToArray();
			}
			return items;
		}
	}



	public static class CustomEnumValues
	{
		public static readonly List<string[]> CustomEnums = new List<string[]>();

		static CustomEnumValues()
		{
			CustomEnums.Add(SelectionTypes);
		}

		public static string[] SelectionTypes =
		{
			"22px","32px","48px", "62px","72px","94px","110px","122px","146px","224px",
		};

	}
	public class AsyncOperation
	{
		private TimeSpan _start;

		private Exception ex = null;
		private volatile bool completed = false;

		public volatile string Title = string.Empty;
		public volatile string ItemName = string.Empty;

		public volatile float Progress = 0;

		public void SetProgress(int current,int max)
		{
			Progress = (float)current / (float)max;
		}
		public void SetProgress(string item, int current, int max)
		{
			ItemName = item;
			Progress = (float)current / (float)max;
		}

		public bool Completed
		{
			get
			{
				if (ex != null)
					ExceptionDispatchInfo.Capture(ex).Throw();

				return completed;
			}
		}

		public bool Cancelled { get; private set; }


		private Action onCancelled = null;

		public void Cancel()
		{
			Cancelled = true;
			if (onCancelled != null)
				onCancelled();
		}

		public TimeSpan ElaspedTime => DateTime.Now.TimeOfDay - _start;

		public AsyncOperation(Action<AsyncOperation> action, Action onCancelled = null)
		{
			this.onCancelled = onCancelled;

			_start = DateTime.Now.TimeOfDay;

			Task.Run(() =>
			{
				try
				{
					action(this);
				}
				catch (Exception ex)
				{
					this.ex = ex;
					System.Diagnostics.Debugger.Break();
				}
				completed = true;
			}).ConfigureAwait(false);
		}

	}


	public interface ITreeViewItem
	{
		string Path { get; set; }
		GuiTexture Preview { get; }
	}

	public interface IInteractableTreeViewItem
	{
		void Activate();
	}

}
