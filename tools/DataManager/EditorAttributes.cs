using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace DataManager
{
	[AttributeUsage(AttributeTargets.Property, AllowMultiple = false, Inherited = true)]
	public class SectionAttribute : Attribute
    {
		public readonly string Name;
		public SectionAttribute(string name) { Name = name; }
    }

	[AttributeUsage(AttributeTargets.Property, AllowMultiple = false, Inherited = true)]
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

	public class SupplyEditorAttribute : EditorAttribute
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

	[AttributeUsage(AttributeTargets.Field , AllowMultiple = false, Inherited = true)]
	public class UniqueKeyAttribute : Attribute
	{
		public string Name { get; set; }
	}

	public class ImageFrameEditorAttribute : EditorAttribute
	{
		public readonly string ImagePath;

		public ImageFrameEditorAttribute(string imagePath)
		{
			ImagePath = imagePath;
		}
	}

	public class ArrayEditorAttribute : EditorAttribute
	{
		public readonly int ArraySize;
		public ArrayEditorAttribute(int arraySize)
		{
			ArraySize = arraySize;
		}

	}

	public class ListEditorAttribute : EditorAttribute
	{

	}
}
