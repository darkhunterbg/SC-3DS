#pragma once


class Camera;

class MinimapPanel {

public :
	bool PointerInputEnabled = true;
	void DrawMinimap(Camera& camera);
};