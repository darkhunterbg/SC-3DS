#pragma once

#include "Scene.h"


enum class MainMenuState {
	MainMenu,
	MultiplayerSelection
};

class MainMenuScene : public Scene {
private:
	class IMainMenuSceneView* _view = nullptr;
	MainMenuState _state;
	class IMainMenuSceneView* _nextView;
	MainMenuState _nextState;

	Scene* _nextScene = nullptr;

	void SwitchView(MainMenuState state, class IMainMenuSceneView* view);
public:
	class Cursor* cursor = nullptr;

	MainMenuScene();
	~MainMenuScene();
	virtual void Start() override;
	virtual void Stop() override;
	virtual void Frame(TimeSlice& frameBudget) override;

	inline MainMenuState GetState() const { return _state; }

	void ToMainMenu();
	void ToMultiplayerConnection();

	void StartGame();
};