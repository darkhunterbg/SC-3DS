using System;
using System.Collections.Generic;
using System.Linq;
using System.Numerics;
using System.Text;
using System.Threading.Tasks;

namespace DataManager.Panels
{
	public interface IGuiPanel
	{
		public void Draw(Vector2 client);
	}
}
