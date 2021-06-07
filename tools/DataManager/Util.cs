using GlobExpressions;
using System;
using System.Collections.Generic;
using System.IO;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace DataManager
{
	public static class Util
	{
		public static TType ShallowCopyProperties<TType>(TType copy) where TType : class
		{
			var instance = Activator.CreateInstance<TType>();

			var props = typeof(TType).GetProperties().Where(t => t.CanRead && t.CanWrite).ToList();

			foreach(var p in props)
			{
				p.SetValue(instance, p.GetValue(copy));
			}

			return instance;
		}

		public static IEnumerable<T> TextFilter<T>(IEnumerable<T> query, string text, Func<T, string> filterBy, bool semiMatch = true)
		{
			if (!string.IsNullOrEmpty(text))
			{
				if (semiMatch &&
					!text.Contains('*') &&
					!text.Contains('?') &&
					!text.Contains('[') &&
					!text.Contains('{'))

					query = query.Where(a => filterBy(a).Contains(text, StringComparison.InvariantCultureIgnoreCase));
				else
				{
					try
					{
						var g = new Glob(text, GlobOptions.Compiled);
						query = query.Where(a => g.IsMatch(filterBy(a)));
					}
					catch { }
				}

			}
			return query;
		}

	}
}
