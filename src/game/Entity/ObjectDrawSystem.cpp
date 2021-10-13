#include "ObjectDrawSystem.h"

#include "../Engine/GraphicsRenderer.h"
#include "../Data/AssetDataDefs.h"
#include"../Camera.h"
#include "../Profiler.h"
#include "EntityManager.h"

#include <cstring>

ObjectDrawSystem::ObjectDrawSystem()
{
	for (int i = 0; i < _entityToIndexMap.size(); ++i)
		_entityToIndexMap[i] = -1;
}

void ObjectDrawSystem::NewComponent(EntityId id)
{
	short index = _drawComponents.size();

	_drawComponents.push_back(DrawComponent());
	_entityToIndexMap[Entity::ToIndex(id)] = index;

	if (id > _maxEntity) _maxEntity = id;
}

DrawComponent& ObjectDrawSystem::GetComponent(EntityId id)
{
	short index = _entityToIndexMap[Entity::ToIndex(id)];
	return _drawComponents[index];
}

DrawComponent* ObjectDrawSystem::TryGetComponent(EntityId id)
{
	short index = _entityToIndexMap[Entity::ToIndex(id)];
	if (index == -1) return nullptr;
	return &_drawComponents[index];
}

void ObjectDrawSystem::RemoveComponent(EntityId id)
{
	short index = _entityToIndexMap[Entity::ToIndex(id)];
	_entityToIndexMap[Entity::ToIndex(id)] = -1;
	_drawComponents.erase(_drawComponents.begin() + index);

	int end = Entity::ToIndex(_maxEntity);
	EntityId last = Entity::None;

	for (int i = 0; i < end; ++i)
	{
		if (_entityToIndexMap[i] > index)
		{
			--_entityToIndexMap[i];
			last = Entity::ToId(i);
		}
	}

	_maxEntity = last;
}

static bool DrawSort(const BatchDrawCommand& a, const BatchDrawCommand& b)
{
	return a.order < b.order;
}

void ObjectDrawSystem::Draw(EntityManager& em, const Camera& camera)
{
	SectionProfiler p("ObjectDraw");

	for (int i = 0; i < Entity::ToIndex(_maxEntity); ++i)
	{
		auto draw = TryGetComponent(Entity::ToId(i));
		if (draw != nullptr && draw->visible)
		{
			draw->position = em.GetPosition(Entity::ToId(i));
			draw->boundingBox.SetCenter(draw->position);
		}
	}

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
