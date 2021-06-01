#pragma once


#include <citro3d.h>
#include "MathLib.h"

struct RenderTarget {
	C3D_RenderTarget* rt = nullptr;
	C3D_Tex* tex = nullptr;
	Vector2Int size;
	C3D_Mtx projMtx;

	inline Vector2Int const GetSize() {

		return size;
	}

	void Init() {
		if (rt->linked)
			Mtx_OrthoTilt(&projMtx, 0.0f, GetSize().x, GetSize().y, 0.0f, 1.0f, -1.0f, true);
		else
			Mtx_Ortho(&projMtx, 0.0f, GetSize().x, GetSize().y, 0.0f, 1.0f, -1.0f, true);
	}
};
