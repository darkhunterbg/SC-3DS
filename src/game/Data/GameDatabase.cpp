#include "GameDatabase.h"
#include "../Engine/AssetLoader.h"
#include "../Debug.h"
#include "../Entity/AnimationSystem.h"

const GameDatabase* GameDatabase::instance = nullptr;

GameDatabase::GameDatabase()
{
	instance = this;
}

const Image& GameDatabase::GetImage(const std::string& path) const
{
	auto r = imageNamesMap.find(path);
	if (r == imageNamesMap.end())
		EXCEPTION("Tried to get invalid image path '%s'", path.data());

	return *r->second;
}

const ImageFrame& GameDatabase::GetCommandIcons(unsigned id) const
{
	return commandIcons->GetFrame(id);
}

void GameDatabase::LoadAssetReferences()
{
	std::vector<const Texture*> textures;

	for (const AtlasDef& atlases : AtlasDefs) {
		std::string name = atlases.GetAtlasName();
		name = "atlases\\" + name;
		auto tex = AssetLoader::LoadTexture(name.data());
		textures.push_back(tex);
	}

	images.reserve(ImageDefs.size());
	frames.reserve(frames.size());

	for (const ImageFrameDef& frameDef : FrameDefs) {
		const Texture* tex = textures[frameDef.atlasId];
		frames.push_back(ImageFrame(*tex, frameDef));
	}

	for (const ImageDef& imageDef : ImageDefs) {
		
		const ImageFrame* f = &frames[imageDef.frameStart];
		images.push_back(Image(f, imageDef));
		const auto& img = images.back();
		imageNamesMap[img.GetName()] = &img;
	}

	commandIcons = imageNamesMap["unit\\cmdbtns\\cmdicons"];

	AnimationSystem::BuildInstructionCache({ AnimInstructionDefs.data(), AnimInstructionDefs.size() });
}
