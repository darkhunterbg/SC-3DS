using Microsoft.Xna.Framework;
using Newtonsoft.Json;
using System;
using System.Collections.Generic;
using System.IO;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace DataManager
{
	public static class AppSettings
	{
		static string SettingsDir = "../../settings/";
		static JsonSerializerSettings serializationSettings;

		static AppSettings()
		{
			if (!Directory.Exists(SettingsDir))
				Directory.CreateDirectory(SettingsDir);

			serializationSettings = new JsonSerializerSettings()
			{
				Formatting = Formatting.Indented
			};
		}

		public static void Save(string fileName, object obj)
		{
			string text = JsonConvert.SerializeObject(obj, serializationSettings);
	
			File.WriteAllText(Path.Combine(SettingsDir,$"{fileName}.json"), text);
		}

		public static T Load<T>(string fileName)
		{
			try
			{
				string text = File.ReadAllText(Path.Combine(SettingsDir, $"{fileName}.json"));
				return JsonConvert.DeserializeObject<T>(text, serializationSettings);
			}
			catch
			{
				return default;
			}
			
		}
	}
}
