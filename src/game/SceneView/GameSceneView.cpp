#include "GameSceneView.h"
#include "../Entity/EntityUtil.h"
#include "../Entity/EntityManager.h"
#include "../Data/GameDatabase.h"
#include "../Game.h"
#include "../Platform.h"
#include "../GUI/GUI.h"
#include "../StringLib.h"
#include "../Engine/AssetLoader.h"
#include "../Profiler.h"
#include "../Scenes/GameScene.h"


GameSceneView::GameSceneView(GameScene* scene) : _scene(scene)
{
	Vector2Int16 size = _scene->GetEntityManager().MapSystem.GetSize();
	_camera.Position = { 0,0 };
	_camera.Limits = { {0,0,}, size };

	_cursor.MultiSelectionEnabled = true;
	_cursor.UsingLimits = true;
	_cursor.DrawMultiSelection = false;
	_cursor.Position = Vector2Int(Game::GetPlatformInfo().Screens[0] / 2);
	_scene->GetEntityManager().OnEntitiesDeleted = [this](auto& a) { OnEntitiesDeleted(a); };

	SetPlayer(_scene->GetEntityManager().MapSystem.ActivePlayer);

}

void GameSceneView::SetPlayer(PlayerId player)
{
	_player = player;
	_scene->GetEntityManager().MapSystem.ActivePlayer = player;
	const PlayerInfo& info = _scene->GetEntityManager().PlayerSystem.GetPlayerInfo(_player);

	_resourceBar.UpdatePlayerInfo(info, true);

	SetDefaultMode();
}
const PlayerInfo& GameSceneView::GetPlayerInfo() const
{
	return _scene->GetEntityManager().PlayerSystem.GetPlayerInfo(_player);
}
Color GameSceneView::GetAlliedUnitColor(EntityId id)
{
	if (EntityUtil::IsAlly(_player, id))
		return Colors::UIDarkGreen;

	if (EntityUtil::IsEnemy(_player, id))
		return Colors::UIDarkRed;

	return Colors::UIDarkYellow;
}
void GameSceneView::OnEntitiesDeleted(const std::vector<EntityId>& entities)
{
	if (_unitSelection.size() == 0) return;

	for (EntityId id : entities)
	{
		for (int i = 0; i < _unitSelection.size(); ++i)
		{
			EntityId unit = _unitSelection[i];
			if (unit == id)
			{
				_unitSelection.erase(_unitSelection.begin() + i);
				--i;
			}
		}
	}
}
void GameSceneView::UpdateSelection()
{
	Rectangle selection;
	if (_cursor.GetScreenSelection(selection))
	{
		_temp.clear();
		Rectangle16 r;
		r.position = _camera.ScreenToWorld(Vector2Int16(selection.position));
		r.SetMax(_camera.ScreenToWorld(Vector2Int16(selection.GetMax())));
		// TODO: unit masking
		_scene->GetEntityManager().KinematicSystem.RectCast(r, _temp);

		bool multiSelect = false;

		for (int i = 0; i < _temp.size(); ++i)
		{
			EntityId id = _temp[i];

			bool remove = false;

			if (!_scene->GetEntityManager().UnitSystem.IsUnit(id)) remove = true;
			if (!_scene->GetEntityManager().DrawSystem.GetComponent(id).visible)  remove = true;


			if (remove)
			{
				_temp.erase(_temp.begin() + i);
				--i;
			}

			if (!multiSelect && EntityUtil::IsAlly(_player, id))
				multiSelect = true;
		}

		if (_temp.size() > 0)
		{
			EntityId prev = Entity::None;
			if (_unitSelection.size() > 0)
				prev = _unitSelection[0];

			_unitSelection.clear();

			if (multiSelect && _temp.size() > 1)
			{
				for (EntityId id : _temp)
				{
					if (EntityUtil::IsAlly(_player, id))
					{
						_unitSelection.push_back(id);
					}
				}
			}
			else
			{
				_unitSelection.push_back(_temp[0]);
			}

			if (_unitSelection.size() > 0)
				OnUnitSelect(_unitSelection[0], _unitSelection[0] != prev);
			else
				OnUnitSelect(Entity::None, true);
		}
	}

	// Dynamic selection update;
	for (int i = 0; i < _unitSelection.size(); ++i)
	{
		EntityId id = _unitSelection[i];
		if (!_scene->GetEntityManager().DrawSystem.GetComponent(id).visible)
		{
			_unitSelection.erase(_unitSelection.begin() + i);
			--i;
		}
	}

	Color selectionColor = Colors::White;

	if (_unitSelection.size())
	{
		selectionColor = GetAlliedUnitColor(_unitSelection[0]);
	}
	else
	{
		_selectTargetAbility = nullptr;
	}

	_scene->GetEntityManager().DrawSystem.UpdateSelection(_unitSelection, selectionColor);
}
void GameSceneView::ContextActionCheck()
{
	if (_cursorOverUI)return;

	bool context = Game::GetInput().Cursor.Context.IsActivated();
	if (!context) return;

	if (IsTargetSelectionMode())
	{
		SetDefaultMode();
		return;
	}

	Vector2Int16  worldPos = _camera.ScreenToWorld(Vector2Int16(_cursor.Position));
	ActivateContextAbilityAt(worldPos);
}
void GameSceneView::TargetActionCheck()
{
	if (_cursorOverUI) return;

	bool context = Game::GetInput().Cursor.Select.IsActivated();
	if (!context) return;

	if (_unitSelection.size() == 0 || !EntityUtil::IsAlly(_player, _unitSelection[0])) return;

	if (!IsTargetSelectionMode()) return;

	Vector2Int16 worldPos = _camera.ScreenToWorld(Vector2Int16(_cursor.Position));
	ActivateAbilityAt(worldPos);
}
void GameSceneView::ActivateAbilityAt(Vector2Int16 worldPos)
{
	if (!IsTargetSelectionMode()) return;

	EntityId target = GetEntityUnderPosition(worldPos);
	if (target != Entity::None)
	{
		for (EntityId id : _unitSelection)
			EntityUtil::ActivateAbility(id, *_selectTargetAbility, target);
		OnAbilityActivated();
		return;
	}

	if (_selectTargetAbility->CanTargetPosition())
	{
		for (EntityId id : _unitSelection)
		{
			EntityUtil::ActivateAbility(id, *_selectTargetAbility, worldPos);
		}
		OnAbilityActivated();
	}
}
void GameSceneView::ActivateContextAbilityAt(Vector2Int16 worldPos)
{
	if (_unitSelection.size() == 0 || !EntityUtil::IsAlly(_player, _unitSelection[0])) return;

	EntityId entity = GetEntityUnderPosition(worldPos);

	if (entity != Entity::None)
	{
		for (EntityId id : _unitSelection)
		{
			const AbilityDef* ability = EntityUtil::GetUnitDefaultAbility(id, entity);
			EntityUtil::ActivateAbility(id, *ability, entity);
		}
	}
	else
	{
		for (EntityId id : _unitSelection)
		{
			const AbilityDef* ability = EntityUtil::GetUnitDefaultAbility(id, worldPos);
			EntityUtil::ActivateAbility(id, *ability, worldPos);
		}
	}
}
void GameSceneView::OnAbilityActivated()
{
	SetDefaultMode();

	if (_unitSelection.size() == 0 || !EntityUtil::IsAlly(_player, _unitSelection[0])) return;

	bool played = _scene->GetEntityManager().SoundSystem.PlayUnitChat(_unitSelection[0], UnitChatType::Command);

	if (played)
		_unitPortrait.ChatUnit(_unitSelection[0], false);
}
void GameSceneView::OnUnitSelect(EntityId id, bool newSelection)
{
	if (id != Entity::None && EntityUtil::IsAlly(_player, id))
	{
		bool played = _scene->GetEntityManager().SoundSystem.PlayUnitChat(id, UnitChatType::Selected);

		if (played)
			_unitPortrait.ChatUnit(id, newSelection);
	}
}
void GameSceneView::SetDefaultMode()
{
	_selectTargetAbility = nullptr;
}

void GameSceneView::Update()
{
	_camera.Size = Game::GetPlatformInfo().Screens[0];
	// Componensate for UI
	if (Game::GetPlatformInfo().Type != PlatformType::Nintendo3DS)
	{
		_camera.Size.y -= 96;
	}

	_camera.Update(&_cursor);

	_cursor.UsingLimits = _cursor.IsHolding();
	_cursor.Limits = { {0,0 }, Vector2Int(_camera.Size) };
	_cursor.MultiSelectionEnabled = !_cursorOverUI && !IsTargetSelectionMode();
	_minimap.PointerInputEnabled = !_cursor.IsHolding();

	_cursor.TargetMode = (!_cursorOverUI || _cursorOverMinimap) && IsTargetSelectionMode();
	_cursor.GameUpdate();


	EntityId hover = GetEntityUnderCursor();

	if (hover != Entity::None)
	{
		// TODO: entity might not be part of drawing system
		if (_scene->GetEntityManager().DrawSystem.GetComponent(hover).visible)
		{
			if (EntityUtil::IsAlly(_player, hover))
				_cursor.SetUnitHover(CursorHoverState::Green);
			else if (EntityUtil::IsEnemy(_player, hover))
				_cursor.SetUnitHover(CursorHoverState::Red);
			else
				_cursor.SetUnitHover(CursorHoverState::Yellow);
		}
	}

	ContextActionCheck();
	TargetActionCheck();
	UpdateSelection();

	const PlayerInfo& info = _scene->GetEntityManager().PlayerSystem.GetPlayerInfo(_player);

	_resourceBar.UpdatePlayerInfo(info, false);
}

EntityId GameSceneView::GetEntityUnderCursor()
{
	if (!_cursorOverUI)
	{
		Vector2Int16 worldPos = _camera.ScreenToWorld(Vector2Int16(_cursor.Position));
		return GetEntityUnderPosition(worldPos);
	}
	return Entity::None;
}
EntityId GameSceneView::GetEntityUnderPosition(Vector2Int16 worldPos)
{
	// TODO: raycast masking
	EntityId hover = _scene->GetEntityManager().KinematicSystem.PointCast(worldPos);
	return hover;
}

void GameSceneView::Draw()
{
	//SectionProfiler p("GUI");

	if (_unitSelection.size() > 0 && _scene->GetEntityManager().SoundSystem.IsChatPlaying())
	{
		if (EntityUtil::IsAlly(_player, _unitSelection[0]))
			_unitPortrait.ChatUnit(_unitSelection[0], false);
	}

	DrawMainScreen();

	if (Platform::GetPlatformInfo().Type != PlatformType::Nintendo3DS)
		return;

	DrawSecondaryScreen();
}


void GameSceneView::OnPlatformChanged()
{
	GUI::UseScreen(ScreenId::Top);

	_cursor.Position = GUI::GetScreenSize() / 2;
}

void GameSceneView::DrawMainScreen()
{
	_cursorOverMinimap = false;

	GUI::UseScreen(ScreenId::Top);

	auto& info = GetPlayerInfo();

	const RaceDef* raceDef = GameDatabase::instance->GetRace(info.race);

	if (raceDef == nullptr) raceDef = GameDatabase::instance->GetRace(RaceType::Terran);

	if (_cursor.IsHolding())
	{
		Rectangle rect = _cursor.GetHoldRect();
		Util::DrawTransparentRectangle(rect, 1, Colors::UIGreen);
	}

	if (Platform::GetPlatformInfo().Type != PlatformType::Nintendo3DS)
	{
		GUIImage::DrawImageFrame(raceDef->ConsoleSprite);

		GUI::BeginRelativeLayout({ 6,-4 }, { 128,128 }, GUIHAlign::Left, GUIVAlign::Bottom);
		DrawMinimap();
		GUI::EndLayout();

		GUI::BeginRelativeLayout({ 150,-3 }, { 252,90 }, GUIHAlign::Left, GUIVAlign::Bottom);
		_selectionPanel.Draw(_unitSelection, *raceDef);
		GUI::EndLayout();

		// Block mouse multiselection on UI panel
		_cursorOverUI = false;

		if (!_cursor.IsHolding())
		{
			GUI::BeginRelativeLayout({ 0,0 }, { 160, 160 }, GUIHAlign::Left, GUIVAlign::Bottom);

			if (GUI::GetLayoutSpace().Contains(_cursor.Position))
				_cursorOverUI = true;
			GUI::EndLayout();

			GUI::BeginRelativeLayout({ 0,0 }, { 400, 112 }, GUIHAlign::Center, GUIVAlign::Bottom);
			if (GUI::GetLayoutSpace().Contains(_cursor.Position))
				_cursorOverUI = true;
			GUI::EndLayout();

			GUI::BeginRelativeLayout({ 0,0 }, { 160, 140 }, GUIHAlign::Right, GUIVAlign::Bottom);
			if (GUI::GetLayoutSpace().Contains(_cursor.Position))
				_cursorOverUI = true;
			GUI::EndLayout();
		}


		GUI::BeginRelativeLayout({ -169,-19 }, { 54,50 }, GUIHAlign::Right, GUIVAlign::Bottom);
		DrawPortrait();
		GUI::EndLayout();

		GUI::BeginRelativeLayout({ -5,-3 }, { 130,120 }, GUIHAlign::Right, GUIVAlign::Bottom);
		DrawCommandPanel();
		GUI::EndLayout();
	}
	else
	{
		GUI::BeginRelativeLayout({ 0,0 }, Vector2Int(raceDef->ConsoleUpperSprite.size), GUIHAlign::Center, GUIVAlign::Bottom);
		GUIImage::DrawImageFrame(raceDef->ConsoleUpperSprite);
		GUI::EndLayout();
	}

	Vector2Int barSize = GUI::GetScreenSize();
	barSize.y = 14;
	GUI::BeginRelativeLayout({ 0,2 }, barSize, GUIHAlign::Center, GUIVAlign::Top);
	_resourceBar.Draw(info);
	GUI::EndLayout();


	_cursor.Draw();
}
void GameSceneView::DrawSecondaryScreen()
{
	GUI::UseScreen(ScreenId::Bottom);

	GUIImage::DrawColor(Colors::Black);

	auto& info = GetPlayerInfo();

	const RaceDef* raceDef = GameDatabase::instance->GetRace(info.race);

	if (raceDef == nullptr) raceDef = GameDatabase::instance->GetRace(RaceType::Terran);

	GUI::BeginRelativeLayout({ -18,24 }, { 54,50 }, GUIHAlign::Right, GUIVAlign::Top);
	DrawPortrait();
	GUI::EndLayout();

	GUIImage::DrawImageFrame(raceDef->ConsoleLowerSprite);

	GUI::BeginRelativeLayout({ 7,-3 }, { 113,113 }, GUIHAlign::Left, GUIVAlign::Bottom);
	DrawMinimap();
	GUI::EndLayout();

	GUI::BeginRelativeLayout({ 0,2 }, { 234,86 }, GUIHAlign::Left, GUIVAlign::Top);
	_selectionPanel.Draw(_unitSelection, *raceDef);
	GUI::EndLayout();

	GUI::BeginRelativeLayout({ -5,-3 }, { 130,120 }, GUIHAlign::Right, GUIVAlign::Bottom);
	DrawCommandPanel();
	GUI::EndLayout();
}

void GameSceneView::DrawPortrait()
{
	EntityId id = Entity::None;

	if (_unitSelection.size() > 0)
	{
		id = _unitSelection[0];

		_unitPortrait.Draw(id);

		if (GUI::IsLayoutPressed())
		{
			_camera.SetPositionRestricted(_scene->GetEntityManager().GetPosition(id));
		}
	}
}
void GameSceneView::DrawCommandPanel()
{
	auto& info = GetPlayerInfo();

	const RaceDef* raceDef = GameDatabase::instance->GetRace(info.race);

	if (raceDef == nullptr) raceDef = GameDatabase::instance->GetRace(RaceType::Terran);

	if (IsTargetSelectionMode())
	{
		auto cmd = _commandPanel.DrawAbilityCommandsAndSelect(_selectTargetAbility, *raceDef);
		if (cmd != nullptr)
		{
			// Cancelled
			SetDefaultMode();
		}
	}
	else
	{
		if (_unitSelection.size() > 0 && EntityUtil::IsAlly(_player, _unitSelection[0]))
		{
			auto cmd = _commandPanel.DrawUnitCommandsAndSelect(_unitSelection[0], *raceDef);
			if (cmd != nullptr)
			{
				if (cmd->IsAbility())
				{
					if (cmd->ability->HasTargetSelection())
						_selectTargetAbility = cmd->ability;
					else
					{
						for (EntityId id : _unitSelection)
							EntityUtil::ActivateAbility(id, *cmd->ability);

						OnAbilityActivated();
					}
				}
			}
		}
	}
}
void GameSceneView::DrawMinimap()
{
	_cursorOverMinimap = false;

	if (GUI::GetLayoutSpace().Contains(_cursor.Position))
		_cursorOverMinimap = true;
	auto result = _minimap.DrawMinimapAndAcitvate(_camera);

	if (result.isActivate)
	{
		if (!IsTargetSelectionMode())
			_camera.SetPositionRestricted(result.worldPos);
		else
			ActivateAbilityAt(result.worldPos);

	}

	if (result.isAlternativeActivate)
	{
		if (!IsTargetSelectionMode())
		{
			ActivateContextAbilityAt(result.worldPos);
		}
	}
}