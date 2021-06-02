using System;
using System.Collections.Generic;
using System.Linq;
using System.Runtime.ExceptionServices;
using System.Text;
using System.Threading.Tasks;

namespace DataManager
{
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
					action((f)=>
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
