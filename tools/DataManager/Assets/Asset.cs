using CsvHelper.Configuration.Attributes;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace DataManager.Assets
{
	public abstract class Asset
	{
		[Ignore]
		public abstract string AssetName { get; }

		[Ignore]
		public virtual GuiTexture Preview => null;

		[Ignore]
		[UniqueKey]
		public readonly int Id;

		static int id = 0;

		public Asset() { id = ++id; }

		public virtual void OnAfterDeserialize() { }

		public override string ToString()
		{
			return AssetName;
		}
	}

}
