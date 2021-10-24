#pragma once

#include "InputDevices.h"


class InputManager {

private:

	InputManager(const InputManager&) = delete;
	InputManager& operator=(const InputManager&) = delete;
	static InputManager instance;

public:

	static Gamepad Gamepad;
	static Pointer Pointer;

	static void Update();
};

