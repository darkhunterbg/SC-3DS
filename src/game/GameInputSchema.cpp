#include "GameInputSchema.h"
#include "Engine/InputManager.h"
#include "Debug.h"
#include "Platform.h"
#include "Game.h"

InputCommand::InputCommand(const char* name) :_cmdName(name)
{
}

void InputCommand::AddCommand(std::function<void(Vector2&)> cmd)
{
	GAME_ASSERT(_commandCount < MaxCommands, "Cannot add more than i% commands to input '%s'", MaxCommands, _cmdName);
	_commands[_commandCount++] = cmd;
}

void InputCommand::ClearCommands()
{
	_commandCount = 0;
}

Vector2 InputCommand::VectorValue()  const
{
	Vector2 result = { };
	for (int i = 0; i < _commandCount; ++i)
	{
		_commands[i](result);

		if (result.LengthSquared())
			break;
	}
	return result;
}

bool InputCommand::IsActivated() const
{
	return VectorValue().LengthSquared() != 0;
}

float InputCommand::LinearValue() const
{
	return VectorValue().Length();
}


void GameInputSchema::RegisterCommand(InputCommand& cmd)
{
	Commands[cmd.GetName()] = &cmd;
}

bool GameInputSchema::IsUsingMouse()
{
	return Game::GetPlatformInfo().PointerIsCursor;
}

GameInputSchema::GameInputSchema()
{
	RegisterCommand(Cursor.Move);
}

void GameInputSchema::InitDefault()
{
	for (auto& cmd : Commands)
		cmd.second->ClearCommands();

	Cursor.Move.AddCommand([](Vector2& r) {
		if (InputManager::Gamepad.IsButtonUp(GamepadButton::L))
			r = InputManager::Gamepad.CPad();
		});
	Cursor.Select.AddCommand([](Vector2& r) {
		r = Vector2(InputManager::Gamepad.IsButtonPressed(GamepadButton::Y));
		});

	Camera.Move.AddCommand([](Vector2& r) {
		r = InputManager::Gamepad.CStick();
		});
	Camera.Move.AddCommand([](Vector2& r) {
		if (InputManager::Gamepad.IsButtonDown(GamepadButton::L))
			r = InputManager::Gamepad.CPad();
		});
	Camera.Zoom.AddCommand([](Vector2& r) {
		r = Vector2(InputManager::Gamepad.IsButtonPressed(GamepadButton::R));
		});

	Common.Comfirm.AddCommand([](Vector2& r) {
		r = Vector2(InputManager::Gamepad.IsButtonPressed(GamepadButton::A));
		});
	Common.Cancel.AddCommand([](Vector2& r) {
		r = Vector2(InputManager::Gamepad.IsButtonPressed(GamepadButton::B));
		});

	Cheats.ToggleFoW.AddCommand([](Vector2& r) {
		r = Vector2(InputManager::Gamepad.IsButtonPressed(GamepadButton::Select));
		});
}
