using System;
using System.Collections.Generic;
using System.Linq;
using System.Runtime.ExceptionServices;
using System.Text;
using System.Threading.Tasks;

namespace DataManager
{

	public enum CustomEnumType
	{
		SelectionTypes
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
