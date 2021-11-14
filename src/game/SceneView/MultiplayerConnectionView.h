#pragma once

#include "IMainMenuSceneView.h"
#include "../Widgets/Cursor.h"
#include <array>

class MultiplayerConnectionView : public IMainMenuSceneView {
	double _time = 0;

	bool _hideStart = false;

	bool IsSlideDone() const;

	bool _hostClientModal = false;
	bool _connectIPModal = false;

	std::array<char, 64> _textInput;
public:
	MultiplayerConnectionView(class MainMenuScene* scene);
	virtual  ~MultiplayerConnectionView() override;
	virtual void Draw() override;
	virtual void OnShow() override;
	virtual bool DoneHiding() override;
};