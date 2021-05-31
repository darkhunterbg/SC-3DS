#include "InputManager.h"

Gamepad InputManager::Gamepad;
Pointer InputManager::Pointer;

void InputManager::Update()
{
	Gamepad.Update();
	Pointer.Update();
}
