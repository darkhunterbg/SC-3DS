#pragma once

#include "Game.h"
#include "ImGui/imgui.h"

#include "AbstractPlatform.h"

#include "Engine/AudioManager.h"

#include "SceneView/GameSceneView.h"
#include "Entity/EntityManager.h"

#include "Scenes/GameScene.h"
#include "Scenes/BootScene.h"
#include "Scenes/MainMenuScene.h"
#include "Scenes/VideoPlaybackScene.h"

#include <functional>

extern AbstractPlatform abstractPlatform;
extern bool mute;

void DrawSettings()
{
	if (!ImGui::Begin("Settings"))
	{
		ImGui::End(); return;
	}

	int platformIndex = -1;


	int i = 0;
	for (auto& p : AbstractPlatform::Platforms)
	{
		if (p.Name == abstractPlatform.Name)
		{
			platformIndex = i; break;
		}
		++i;
	}

	ImGui::SetNextItemWidth(250);
	if (ImGui::Combo("Platform", &platformIndex, AbstractPlatform::PlatformsString.data(), AbstractPlatform::Platforms.size()))
	{
		abstractPlatform = AbstractPlatform::Platforms[platformIndex];
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

	static std::vector<const char*> sceneNames;

	sceneNames.clear();

	sceneNames.push_back(NAMEOF(VideoPlaybackScene));
	sceneNames.push_back(NAMEOF(BootScene));
	sceneNames.push_back(NAMEOF(MainMenuScene));
	sceneNames.push_back(NAMEOF(GameScene));

	static std::vector < std::function<void(void)>> sceneLoadMethods;
	sceneLoadMethods.clear();
	sceneLoadMethods.push_back([]() { Game::SetCurrentScene(new VideoPlaybackScene("Smk\\Blizzard", []() {
		Game::SetCurrentScene(new BootScene());
		})); });
	sceneLoadMethods.push_back([]() { Game::SetCurrentScene(new BootScene()); });
	sceneLoadMethods.push_back([]() { Game::SetCurrentScene(new MainMenuScene()); });
	sceneLoadMethods.push_back([]() { Game::SetCurrentScene(new GameScene()); });

	int sceneIndex = -1;
	for (int i = 0; sceneNames.size(); ++i)
	{
		if (Game::GetCurrentScene()->GetId() == std::string(sceneNames[i]))
		{
			sceneIndex = i;
			break;
		}
	}


	ImGui::SetNextItemWidth(250);
	if (ImGui::Combo("Scene", &sceneIndex, sceneNames.data(), sceneNames.size()))
	{
		sceneLoadMethods[sceneIndex]();
	}

	ImGui::Separator();

	if (Game::GetCurrentScene()->GetId() == NAMEOF(GameScene))
	{
		GameScene* scene = static_cast<GameScene*>(Game::GetCurrentScene());

		ImGui::Checkbox("Show Colliders", &scene->GetEntityManager().KinematicSystem.ShowColliders);

		if (ImGui::Button("Kill Seleced Units"))
		{
			scene->GetEntityManager().DeleteEntities(scene->GetView().GetSelection(), false);
		}

		ImGui::Separator();

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
		if (ImGui::Combo("Player", &player, playersString.data(), players.size(), 500))
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
