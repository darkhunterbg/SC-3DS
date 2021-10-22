#include "Platform.h"

#ifdef _3DS
#include <3ds.h>
#endif


void* Platform::PlatformAlloc(unsigned size)
{
#ifdef _3DS
	return linearAlloc(size);
#else
	return malloc(size);
#endif
}
void Platform::PlatformFree(void* ptr)
{
#ifdef _3DS
	return linearFree(ptr);
#else
	return free(ptr);
#endif
}