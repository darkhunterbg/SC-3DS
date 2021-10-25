#if _WIN32
#define WIN32_LEAN_AND_MEAN
#include  <Windows.h>

bool Win32GetCursorPosition(int&x ,int& y)
{
	POINT point;
	bool success = GetCursorPos(&point);
	if(success)
	{
		x = point.x;
		y = point.y;
	}
	return success;
}

#endif