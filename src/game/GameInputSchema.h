#pragma once

#include <unordered_map>
#include <string>
#include <array>
#include <functional>
#include "MathLib.h"

class InputCommand {

	static constexpr const int MaxCommands = 4;

private:
	const char* _cmdName;
	std::array<std::function<void(Vector2&)>, MaxCommands> _commands;
	int _commandCount = 0;

public:

	InputCommand(const char* name);

	void AddCommand(std::function<void(Vector2&)> cmd);
	void ClearCommands();

	bool IsActivated() const;
	float LinearValue() const;
	Vector2 VectorValue() const;


	inline const char* GetName() const { return _cmdName; }
};


class GameInputSchema {
private:
	void RegisterCommand( InputCommand& cmd);
public:
	std::unordered_map<std::string, InputCommand*> Commands;

	GameInputSchema();

	bool IsUsingMouse();
	void InitDefault();

	struct CursorCmd {
		InputCommand Move = InputCommand("Cursor.Move");
		InputCommand Select = InputCommand("Cursor.Select");
		InputCommand Hold = InputCommand("Cursor.Hold");
	} Cursor;

	struct CameraCmd {
		InputCommand Move = InputCommand("Camera.Move");
		InputCommand Zoom = InputCommand("Camera.Zoom");
	} Camera;

	struct CommonCmd {
		InputCommand Comfirm = InputCommand("Common.Comfirm");
		InputCommand Cancel = InputCommand("Common.Cancel");
	} Common;

	struct CheatsCmd {
		InputCommand ToggleFoW = InputCommand("Cheats.ToggleFow");
	} Cheats;


};