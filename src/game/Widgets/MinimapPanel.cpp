#include "MinimapPanel.h"

#include "../Camera.h"
#include "../MathLib.h"
#include "../GUI/GUI.h"
#include "../Entity/EntityUtil.h"
#include "../Entity/EntityManager.h"


MinimapPanel::ActivatedResult MinimapPanel::DrawMinimapAndAcitvate(Camera& camera)
{
	Rectangle dst = GUI::GetLayoutSpace();
	EntityUtil::GetManager().MapSystem.DrawMinimap(dst);

	Rectangle camRect = camera.GetRectangle();
	Vector2 min = Vector2(camRect.GetMin());

	Vector2 minimapUpscale = Vector2(EntityUtil::GetManager().MapSystem.GetSize()) / Vector2(dst.size);
	min /= minimapUpscale;
	min += Vector2(dst.position);
	Vector2 s = Vector2(camRect.size) / minimapUpscale;

	Rectangle rect;
	rect.position = Vector2Int(min) - 1;
	rect.size = Vector2Int(s) + 2;

	Util::DrawTransparentRectangle(rect, 1, Colors::White);

	if (PointerInputEnabled )
	{
		Vector2Int pos = GUI::GetPointerPosition() - dst.position;

		if (GUI::IsLayoutPressed())
		{
			MinimapPanel::ActivatedResult result;
			result.isPressed = true;
			result.worldPos = Vector2Int16(Vector2(pos) * minimapUpscale);
			return result;
		}

		if (GUI::IsLayoutActivated())
		{
			MinimapPanel::ActivatedResult result;
			result.isActivate = true;
			result.worldPos = Vector2Int16(Vector2(pos) * minimapUpscale);
			return result;
		}
		if (GUI::IsLayoutActivatedAlt())
		{
			MinimapPanel::ActivatedResult result;
			result.isAlternativeActivate = true;
			result.worldPos = Vector2Int16(Vector2(pos) * minimapUpscale);
			return result;
		}
	
	}

	return MinimapPanel::ActivatedResult{};
}
