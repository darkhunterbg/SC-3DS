using System;
using System.Collections.Generic;
using System.Linq;
using System.Runtime.ExceptionServices;
using System.Text;
using System.Text.RegularExpressions;
using System.Threading.Tasks;

namespace DataManager
{
	public enum RaceEnum
	{
		Zerg,
		Terran,
		Protoss
	}

	public enum MoveControlType
	{
		FlingyData,
		Script
	}

	public enum CustomEnumType
	{
		SelectionTypes
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

		private Exception ex = null;
		private volatile bool completed = false;

		public float Progress { get; private set; }

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

		public AsyncOperation(Action<Action<float>> action, Action onCancelled = null)
		{
			this.onCancelled = onCancelled;
			Task.Run(() =>
			{
				try
				{
					action((f) =>
					{
						Progress = f;
					});
				}
				catch (Exception ex)
				{
					this.ex = ex;
				}
				completed = true;
			}).ConfigureAwait(false);
		}
	}
}
