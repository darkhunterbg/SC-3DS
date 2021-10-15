#include "BootScene.h"
#include "../Engine/AssetLoader.h"
#include "../Engine/GraphicsRenderer.h"
#include "../Data/GameDatabase.h"

const Image* _title;

BootScene::BootScene()
{
	_title = &GameDatabase::instance->GetImage("extracted\\glue\\title\\title");
}
void BootScene::Start()
{
	
}

void BootScene::Draw()
{
	GraphicsRenderer::DrawOnScreen(ScreenId::Top);

	auto& frame = _title->GetFrame(0);

	GraphicsRenderer::Draw(frame, { 0,0 }, Colors::White);
}

BootScene::~BootScene()
{

}
