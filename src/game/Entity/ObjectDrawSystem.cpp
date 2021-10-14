#include "ObjectDrawSystem.h"

#include "../Engine/GraphicsRenderer.h"
#include "../Data/AssetDataDefs.h"
#include"../Camera.h"
#include "../Profiler.h"
#include "EntityManager.h"

#include <cstring>

ObjectDrawSystem::ObjectDrawSystem()
{
}

static bool DrawSort(const BatchDrawCommand& a, const BatchDrawCommand& b)
{
	return a.order < b.order;
}

void ObjectDrawSystem::UpdatePositions(EntityManager& em)
{
	for (EntityId id : _drawComponents.GetEntities())
	{
		auto& draw = _drawComponents.GetComponent(id);
		if (draw.visible)
		{
			draw.position = em.GetPosition(id);
			draw.boundingBox.SetCenter(draw.position);
		}
	}
}

void ObjectDrawSystem::Draw( const Camera& camera)
{
	SectionProfiler p("ObjectDraw");

	_drawCmd.clear();

	Rectangle16 camBB = camera.GetRectangle16();

	float camMul = 1.0f / camera.Scale;
	Color32 shadColor = Color32(0, 0, 0, 0.5f);
	Vector2 scale[] = { {camMul,camMul},{-camMul,camMul} };

	for (const DrawComponent& draw : _drawComponents)
	{
		if (!draw.visible) continue;

		if (!camBB.Intersects(draw.boundingBox)) continue;

		int depth = draw.depth * 10'000'000 + draw.position.y * 1000 + draw.position.x * 4;

		BatchDrawCommand cmd;

		cmd.scale = scale[draw.hFlip];

		if (draw.shadow != nullptr)
		{
			cmd.order = depth + 1;
			cmd.sprite = draw.shadow;
			cmd.position = draw.position + draw.shadowOffset;
			cmd.position -= camBB.position;
			cmd.position /= camera.Scale;
			cmd.color = shadColor;
			_drawCmd.push_back(cmd);
		}

		cmd.order = depth + 2;
		cmd.sprite = &draw.sprite;
		cmd.position = draw.position + draw.offset;
		cmd.position -= camBB.position;
		cmd.position /= camera.Scale;

		cmd.color = Color32(Colors::White);

		_drawCmd.push_back(cmd);

		if (draw.colorMask != nullptr)
		{
			cmd.order = depth + 3;
			cmd.sprite = draw.colorMask;
			cmd.color = draw.color;
			_drawCmd.push_back(cmd);
		}
	}

	std::sort(_drawCmd.begin(), _drawCmd.end(), DrawSort);

	GraphicsRenderer::BufferDraw(_drawCmd);
}

void ObjectDrawSystem::DeleteEntities(std::vector<EntityId>& entities)
{
	_drawComponents.DeleteComponents(entities);
}

size_t ObjectDrawSystem::ReportMemoryUsage()
{
	size_t size = _drawCmd.capacity() * sizeof(BatchDrawCommand);
	size += _drawComponents.GetMemoryUsage();

	return size;
}
