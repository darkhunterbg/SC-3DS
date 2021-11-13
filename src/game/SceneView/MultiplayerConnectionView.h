#pragma once

#include "IMainMenuSceneView.h"
#include "../Widgets/Cursor.h"

class MultiplayerConnectionView : public IMainMenuSceneView {
	double _time = 0;

	bool _hideStart = false;

	bool IsSlideDone() const;

public:
	MultiplayerConnectionView(class MainMenuScene* scene);
	virtual  ~MultiplayerConnectionView() override;
	virtual void Draw() override;
	virtual void OnShow() override;
	virtual bool DoneHiding() override;
};