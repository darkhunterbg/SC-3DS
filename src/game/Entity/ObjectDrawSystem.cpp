#include "ObjectDrawSystem.h"

#include "../Engine/GraphicsRenderer.h"
#include "../Data/AssetDataDefs.h"
#include"../Camera.h"
#include "../Profiler.h"
#include "EntityManager.h"

#include <cstring>

static std::array<Color, 3> GreyHPBarColorPalette = { 0xb0b0b0ff, 0x98948cff, 0x585858ff };
static std::array<Color, 3> YellowHPBarColorPalette = { 0xfccc2cff, 0xdc9434ff, 0xb09018ff };
static std::array<Color, 3> RedHPBarColorPalette = { 0xa80808ff, 0xa80808ff, 0x840404ff };
static std::array<Color, 3> GreenHPBarColorPalette = { 0x249824ff, 0x249824ff, 0x249824ff };
static constexpr Color ShieldColor = 0x0c48ccff;

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

void ObjectDrawSystem::Draw(EntityManager& em, const Camera& camera)
{
	//SectionProfiler p("ObjectDraw");

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
			cmd.order = depth;
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

	for (const auto& sel : _selection)
	{
		if (!sel.showMarker) continue;

		const DrawComponent& draw = _drawComponents.GetComponent(sel.entity);
		if (!draw.visible) continue;

		if (!camBB.Intersects(draw.boundingBox)) continue;

		if (!em.UnitSystem.IsUnit(sel.entity)) continue;

		UnitComponent& unit = em.UnitSystem.GetComponent(sel.entity);

		int depth = draw.depth * 10'000'000 + draw.position.y * 1000 + draw.position.x * 4;

		const auto& selectionImage = unit.def->Art.GetSelectionImage();

		BatchDrawCommand cmd;
		cmd.order = depth + 1;
		cmd.sprite = &selectionImage.GetFrame(0);
		cmd.position = draw.position + selectionImage.GetImageFrameOffset(0, false) + Vector2Int16(0, unit.def->Art.SelectionOffset);
		cmd.position -= camBB.position;
		cmd.position /= camera.Scale;
		cmd.scale = scale[0];
		cmd.color = sel.color;

		_drawCmd.push_back(cmd);
	}

	std::sort(_drawCmd.begin(), _drawCmd.end(), DrawSort);

	GraphicsRenderer::BufferDraw(_drawCmd);

	DrawSelectedBars(em, camera);
}

void ObjectDrawSystem::DrawSelectedBars(EntityManager& em, const Camera& camera)
{
	Rectangle16 camRect = camera.GetRectangle16();

	float camMul = 1.0f / camera.Scale;

	for (const auto& sel : _selection)
	{
		if (!sel.showBar) continue;

		const DrawComponent& draw = _drawComponents.GetComponent(sel.entity);
		if (!draw.visible) continue;

		if (!em.UnitSystem.IsUnit(sel.entity)) continue;

		UnitComponent& unit = em.UnitSystem.GetComponent(sel.entity);

		Vector2Int16 pos = draw.position;
		pos.y += unit.def->Art.BarOffset;
		int barSize = unit.def->Art.BarSize;

		barSize /= camera.Scale;

		int size = barSize * 3 - 1;

		Rectangle16 bb = { {0,0}, Vector2Int16(size, 5) };
		bb.SetCenter(pos);

		if (!camRect.Intersects(bb))
			continue;

		auto* palette = &GreenHPBarColorPalette;


		Vector2Int16 dst = camera.WorldToScreen(pos);
		dst.x -= size >> 1;

		if (unit.HasShield())
			dst.y += 4;


		int hpBarsVisible = (int)std::ceil(((float)unit.health.value * barSize) / (float)unit.maxHealth.value);

		if (hpBarsVisible > 1 && hpBarsVisible == barSize && unit.health != unit.maxHealth)
			--hpBarsVisible;

		if (hpBarsVisible != barSize)
		{
			auto* greyPalette = &GreyHPBarColorPalette;

			Rectangle greyDst = { {Vector2Int(dst) + Vector2Int(1,1) }, {size,1 } };

			for (int i = 0; i < 3; ++i)
			{
				GraphicsRenderer::DrawRectangle(greyDst, Color32(greyPalette->at(i)));
				++greyDst.position.y;
			}

			if (unit.health <= unit.maxHealth / 3)
				palette = &RedHPBarColorPalette;
			else if (unit.health <= (unit.maxHealth << 1) / 3)
				palette = &YellowHPBarColorPalette;
		}


		Rectangle barHp = { Vector2Int(dst) + Vector2Int(1,1), Vector2Int(hpBarsVisible * 3 - 1 ,1) };

		for (int i = 0; i < 3; ++i)
		{
			GraphicsRenderer::DrawRectangle(barHp, Color32(palette->at(i)));
			++barHp.position.y;
		}

		Rectangle16 rect = { dst,Vector2Int16(size + 2,5) };
		Util::DrawTransparentRectangle(rect, 1, Colors::Black);

		Rectangle start = { Vector2Int(dst) + Vector2Int(3,1), Vector2Int(1,3) };

		for (int i = 1; i < barSize; ++i)
		{
			GraphicsRenderer::DrawRectangle(start, Color32(Colors::Black));
			start.position.x += 3;
		}

		// ============= Shield =================

		if (unit.HasShield())
		{
			dst.y -= 4;

			int shieldBarsVisible = (int)std::roundf(((float)unit.shield.value * barSize) / (float)unit.maxShield.value);

			/*	if (shieldBarsVisible > 1 && shieldBarsVisible == barSize && unit.shield != unit.shield)
					--shieldBarsVisible;*/

			Rectangle barHp = { Vector2Int(dst) + Vector2Int(1,1), Vector2Int(shieldBarsVisible * 3 - 1 ,1) };

			if (shieldBarsVisible != barSize)
			{
				auto* greyPalette = &GreyHPBarColorPalette;

				Rectangle greyDst = { {Vector2Int(dst) + Vector2Int(1,1) }, {size,1 } };

				for (int i = 0; i < 3; ++i)
				{
					GraphicsRenderer::DrawRectangle(greyDst, Color32(greyPalette->at(i)));
					++greyDst.position.y;
				}
			}

			for (int i = 0; i < 3; ++i)
			{
				GraphicsRenderer::DrawRectangle(barHp, Color32(ShieldColor));
				++barHp.position.y;
			}

			Rectangle16 rect = { dst,Vector2Int16(size + 2,5) };
			Util::DrawTransparentRectangle(rect, 1, Colors::Black);

			Rectangle start = { Vector2Int(dst) + Vector2Int(3,1), Vector2Int(1,3) };

			for (int i = 1; i < barSize; ++i)
			{
				GraphicsRenderer::DrawRectangle(start, Color32(Colors::Black));
				start.position.x += 3;
			}
		}

	}
}

void ObjectDrawSystem::UpdateSelection(const std::vector<EntityId> selection, Color color)
{
	_selection.clear();
	for (EntityId id : selection)
	{
		_selection.push_back({ id, Color32(color), true , true });
	}
}

void ObjectDrawSystem::RemoveFromSelection(EntityId id, bool onlyMarker)
{
	for (auto i = _selection.begin(); i != _selection.end(); ++i)
	{
		if (i->entity == id)
		{
			if (onlyMarker)
				i->showMarker = false;
			else
				_selection.erase(i);
			return;
		}
	}
}

void ObjectDrawSystem::AddToSelection(EntityId id, Color color, bool onlyMarker)
{
	for (auto& s : _selection)
	{
		if (s.entity == id)
		{
			s.color = Color32(color);
			return;
		}
	}

	_selection.push_back({ id, Color32(color), true, !onlyMarker });
}

void ObjectDrawSystem::DeleteEntities(std::vector<EntityId>& entities)
{
	_drawComponents.DeleteComponents(entities);
}

size_t ObjectDrawSystem::ReportMemoryUsage()
{
	size_t size = _drawCmd.capacity() * sizeof(BatchDrawCommand);
	size += _drawComponents.GetMemoryUsage();
	size += _selection.capacity() * sizeof(ObjectSelection);

	return size;
}
