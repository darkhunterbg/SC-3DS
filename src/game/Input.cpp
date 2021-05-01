#include "Input.h"
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
