#include <SDL.h>
#include <SDL_gpu.h>

#include <filesystem>
#include <thread>

#include "ImGui/imgui.h"
#include "ImGui/imgui_impl_sdl.h"
#include "ImGui/imgui_sdl.h"

#include "Color.h"
#include "Game.h"
#include "MathLib.h"
#include "Debug.h"
#include "AbstractPlatform.h"

#include "Engine/AudioManager.h"
#include "Scenes/GameScene.h"
#include "SceneView/GameSceneView.h"
#include "Entity/EntityManager.h"

#include <vector>


GPU_Image* white;
GPU_Target* screen;
SDL_Window* window;
std::filesystem::path assetDir;
std::filesystem::path userDir;
uint64_t mainTimer;

AbstractPlatform abstractPlatform;

Rectangle touchScreenLocation;
bool mute = false;
bool noThreading = false;


void Toolbar();

int main(int argc, char** argv)
{

	SDL_Init(SDL_INIT_VIDEO | SDL_INIT_JOYSTICK | SDL_INIT_AUDIO);

	GPU_SetRequiredFeatures(GPU_FEATURE_BASIC_SHADERS);
	screen = GPU_Init(1280, 720, GPU_DEFAULT_INIT_FLAGS);

	window = SDL_GetWindowFromID(screen->context->windowID);

	SDL_SetWindowResizable(window, SDL_TRUE);
	SDL_SetWindowTitle(window, "StarCraft");
	SDL_MaximizeWindow(window);

	AbstractPlatform::InitPlatforms(screen);

	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiSDL::Initialize(screen);
	ImGui_ImplSDL2_InitForOpenGL(window, screen->context->context);
	ImGui::StyleColorsDark();

	ImGui::GetIO().FontGlobalScale = 1.5f;

	abstractPlatform = AbstractPlatform::PC(screen);

	abstractPlatform.ApplyPlatform();

	GPU_SetShapeBlendMode(GPU_BLEND_NORMAL);
	white = GPU_CreateImage(1, 1, GPU_FORMAT_RGBA);
	uint8_t data[4] = { 255,255,255,255 };
	GPU_Rect rect = { 0,0,1,1 };
	GPU_UpdateImageBytes(white, &rect, data, sizeof(data));

	assetDir = std::filesystem::current_path().append("cooked_assets").append("win32");// .parent_path();

	if (!std::filesystem::exists(assetDir))
	{
		assetDir = std::filesystem::current_path().parent_path().append("cooked_assets").append("win32");
	}

	if (!std::filesystem::exists(assetDir))
	{
		EXCEPTION("Asset directory not found!");
	}

	userDir = std::filesystem::current_path();
	userDir = userDir.append("User");

	if (!std::filesystem::exists(userDir))
		std::filesystem::create_directories(userDir);

	bool done = false;

	mainTimer = SDL_GetPerformanceCounter();



	GameStartSettings settings = {};
	settings.skipIntro = true;
	settings.loadTestScene = true;

	if (argc > 1)
	{
		std::string a = argv[1];
		if (a == "-mute")
		{
			mute = settings.mute = true;
		}
	}

	Game::Start(settings);


	while (!done)
	{
		ImGui::NewFrame();

		abstractPlatform.UpdateScreens(screen);
		abstractPlatform.UpdateInput();

		Game::FrameStart();

		SDL_Event event;
		while (SDL_PollEvent(&event))
		{
			switch (event.type)
			{
			case SDL_QUIT: done = true; break;

			case SDL_WINDOWEVENT:
				if (event.window.event == SDL_WINDOWEVENT_CLOSE)
					done = true;
				if (event.window.event == SDL_WINDOWEVENT_RESIZED)
				{
					GPU_SetWindowResolution(event.window.data1, event.window.data2);
					abstractPlatform.ApplyPlatform();
					Game::PlatformUpdated();
				}
				break;
			}
		}

		if (done)
			break;


		done = !Game::Update();

		if (done)
			break;

		Game::Draw();

		abstractPlatform.Draw(screen);

		Game::FrameEnd();

		ImGui_ImplSDL2_NewFrame();

		Toolbar();
		ImGui::Render();
		ImGuiSDL::Render(ImGui::GetDrawData());

		GPU_Flip(screen);
	}

	Game::End();


	GPU_Quit();
	SDL_Quit();

	return 0;
}



void Toolbar()
{

	if (!ImGui::Begin("Settings"))
	{
		ImGui::End(); return;
	}

	int index = -1;


	int i = 0;
	for (auto& p : AbstractPlatform::Platforms)
	{
		if (p.Name == abstractPlatform.Name)
		{
			index = i; break;
		}
		++i;
	}


	ImGui::SetNextItemWidth(250);
	if (ImGui::Combo("Platform", &index, AbstractPlatform::PlatformsString.data(), AbstractPlatform::Platforms.size()))
	{
		abstractPlatform = AbstractPlatform::Platforms[index];
		abstractPlatform.ApplyPlatform();
		Game::PlatformUpdated();
	}

	bool audioEnabled = !AudioManager::IsMute();

	ImGui::SetNextItemWidth(250);
	if (ImGui::Checkbox("Audio Enabled", &audioEnabled))
	{
		mute = !audioEnabled;
		AudioManager::SetMute(!audioEnabled);
	}

	ImGui::SetNextItemWidth(250);
	ImGui::Checkbox("Show Performance", &Game::ShowPerformanceStats);

	ImGui::Separator();

	if (Game::GetCurrentScene()->GetId() == NAMEOF(GameScene))
	{
		GameScene* scene = static_cast<GameScene*>(Game::GetCurrentScene());

		int player = 0;

		std::vector<const char*> playersString;
		playersString.clear();

		auto& players = scene->GetEntityManager().PlayerSystem.GetPlayers();

		for (auto& p : players)
		{
			playersString.push_back(p.name.data());
			if (p.id == scene->GetView().GetPlayer())
				player = p.id.i;
		}
		

		ImGui::SetNextItemWidth(250);
		if (ImGui::Combo("Player", &player, playersString.data(), players.size(),500))
		{
			scene->GetView().SetPlayer(players[player].id);
		}

		PlayerId p = players[player].id;

		if (ImGui::Button("Give Resources"))
		{
			scene->GetEntityManager().PlayerSystem.AddMinerals(p, 10000);
			scene->GetEntityManager().PlayerSystem.AddGas(p, 10000);
		}
		ImGui::SameLine();
		if (ImGui::Button("Take Resources"))
		{
			scene->GetEntityManager().PlayerSystem.AddMinerals(p, -10000);
			scene->GetEntityManager().PlayerSystem.AddGas(p, -10000);
		}


		ImGui::Checkbox("Fog of War", &scene->GetEntityManager().MapSystem.FogOfWarVisible);

		ImGui::SameLine();
		if (ImGui::Button("Explore Map"))
		{
			scene->GetEntityManager().PlayerSystem.SetMapKnown(p);
		}
	
		
	}

	ImGui::End();
}

