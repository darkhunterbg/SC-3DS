#include "InputManager.h"
#include "../Debug.h"

Gamepad InputManager::Gamepad;
Pointer InputManager::Pointer;
Keyboard InputManager::Keyboard;
std::string InputManager::TextInput;
int InputManager::TextDelete = 0;

void InputManager::Update()
{
	Gamepad.Update();
	Pointer.Update();
	Keyboard.Update();
}

void InputManager::FrameEnd()
{
	TextInput.clear();
	TextDelete = 0;
}

void InputManager::OnTextInput(const char* text)
{
	TextInput += text;
}

void InputManager::OnTextDelete()
{
	TextDelete++;
}


