#pragma once


#include "MathLib.h"

enum class GamepadButton {
	L, R, ZL, ZR, A, B, X, Y, Start, Select
};


struct GamepadState {
	Vector2 CPad, CStick;
	Vector2Int DPad;

	union {
		struct {
			bool L, R, ZL, ZR;
			bool A, B, X, Y;
			bool Start, Select;
		};
		bool Buttons[10] = { 0,0,0,0,0,0,0,0,0,0 };
	};

};
struct PointerState {
	Vector2Int Position;
	bool Touch = false;
};


class Gamepad {
private:
	GamepadState prevState, currentState;
public:
	void Update();
	inline bool IsButtonDown(GamepadButton button) const {
		return currentState.Buttons[(int)button];
	}
	inline bool IsButtonUp(GamepadButton button) const
	{
		return !currentState.Buttons[(int)button];
	}
	inline bool IsButtonPressed(GamepadButton button) const {
		return !prevState.Buttons[(int)button] && currentState.Buttons[(int)button];
	}
	inline bool IsButtonReleased(GamepadButton button) const {
		return prevState.Buttons[(int)button] && !currentState.Buttons[(int)button];
	}

	inline Vector2 CPad() const { return currentState.CPad; }
	inline Vector2 CStick() const { return currentState.CStick; }
	inline Vector2Int DPad() const { return currentState.DPad; }
};

class Pointer {
private:
	PointerState prevState, currentState;
public:
	void Update();
	inline bool IsDown() const { return currentState.Touch; }
	inline bool IsPressed() const { return !prevState.Touch && currentState.Touch; }
	inline bool IsReleased() const { return prevState.Touch && !currentState.Touch; }
	inline Vector2Int Position() const { return currentState.Position; }
	inline Vector2Int DeltaPosition() const { return currentState.Position - prevState.Position; }
};