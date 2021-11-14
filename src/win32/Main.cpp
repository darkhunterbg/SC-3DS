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

#include "SettingsPanel.h"

#include <vector>


GPU_Image* white;
GPU_Target* screen;
SDL_Window* window;
std::filesystem::path assetDir;
std::filesystem::path userDir;
uint64_t mainTimer;

AbstractPlatform abstractPlatform;

extern void __PlatformInit();
extern void __KeyboardEvent(SDL_Event event, bool down);
extern void __TextInputEvent(SDL_Event event);

bool mute = false;
bool noThreading = false;

int main(int argc, char** argv)
{
	SDL_Init(SDL_INIT_VIDEO | SDL_INIT_JOYSTICK | SDL_INIT_AUDIO);

	GPU_SetRequiredFeatures(GPU_FEATURE_BASIC_SHADERS);
	screen = GPU_Init(1280, 720, GPU_DEFAULT_INIT_FLAGS);

	window = SDL_GetWindowFromID(screen->context->windowID);

	SDL_SetWindowResizable(window, SDL_TRUE);
	SDL_SetWindowTitle(window, "StarCraft");
	SDL_MaximizeWindow(window);

	__PlatformInit();

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
	settings.loadTestScene = false;

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
		abstractPlatform.UpdateInput(window);

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
			case SDL_TEXTINPUT: __TextInputEvent(event); break;
			case SDL_KEYDOWN: __KeyboardEvent(event, true); break;
			case SDL_KEYUP: __KeyboardEvent(event, false); break;
			}
		}

		if (done)
			break;


		done = !Game::Frame();

		if (done)
			break;

		abstractPlatform.Draw(screen);

		ImGui_ImplSDL2_NewFrame();

		DrawSettings();
		ImGui::Render();
		ImGuiSDL::Render(ImGui::GetDrawData());

		GPU_Flip(screen);
	}

	Game::End();


	GPU_Quit();
	SDL_Quit();

	return 0;
}




