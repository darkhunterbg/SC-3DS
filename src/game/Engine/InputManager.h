#pragma once

#include "InputDevices.h"
#include <string>

class InputManager {

private:

	InputManager(const InputManager&) = delete;
	InputManager& operator=(const InputManager&) = delete;
	static InputManager instance;

public:
	static std::string TextInput;
	static int TextDelete;

	static Gamepad Gamepad;
	static Pointer Pointer;
	static Keyboard Keyboard;

	static void Update();
	static void FrameEnd();

	static void OnTextInput(const char* text);
	static void OnTextDelete();
};

