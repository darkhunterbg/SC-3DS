#include "GameDatabase.h"
#include "../Engine/AssetLoader.h"

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
		imageNamesMap[img.GetName()] = images.size() - 1;
	}
}
