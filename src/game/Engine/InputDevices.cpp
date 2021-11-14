#include "InputDevices.h"

#include "Platform.h"

void Gamepad::Update()
{
	prevState = currentState;
	Platform::UpdateGamepadState(currentState);
}

void Pointer::Update()
{
	prevState = currentState;
	Platform::UpdatePointerState(currentState);
}

void Keyboard::Update()
{
	prevState = currentState;
	currentState.keys.clear();
	Platform::UpdateKyeboardState(currentState);
}
