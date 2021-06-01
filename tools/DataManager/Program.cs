using System;

namespace DataManager
{
	public static class Program
	{
		[STAThread]
		static void Main()
		{
			using (var game = new AppGame())
				game.Run();
		}
	}
}
