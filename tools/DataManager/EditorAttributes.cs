using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace DataManager
{
	[AttributeUsage(AttributeTargets.Field | AttributeTargets.Property, AllowMultiple = false, Inherited = true)]
	public abstract class EditorAttribute : Attribute
	{
		public string Name { get; set; }
		public bool ReadOnly { get; set; }
	}


	public class DefaultEditorAttribute : EditorAttribute
	{
		
	}

	public class FrameTimeEditorAttribute : EditorAttribute
	{

	}

	public class IconEditorAttribute : EditorAttribute
	{

	}

	public class CustomEnumEditorAttribute : EditorAttribute
	{
		public CustomEnumType EnumType;

		public CustomEnumEditorAttribute(CustomEnumType enumType)
		{
			EnumType = enumType;
		}
	}

	[AttributeUsage(AttributeTargets.Field | AttributeTargets.Property, AllowMultiple = false, Inherited = true)]
	public class UniqueKeyAttribute : Attribute
	{
		public string Name { get; set; }
	}



}
