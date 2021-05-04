#include "Platform.h"

#include <citro3d.h>
#include <citro2d.h>
#include <3ds.h>
#include <string>
#include <vector>
#include <fstream>

#include "StringLib.h"
#include "Audio.h"
#include "NDSAudioChannel.h"

struct RenderTarget {
	C3D_RenderTarget* rt;
	C3D_Tex* tex;
};




extern C3D_RenderTarget* screens[2];
extern std::string assetDir;
extern C2D_TextBuf textBuffer;
extern u64 mainTimer;
extern std::vector<NDSAudioChannel*> audioChannels;
static C3D_RenderTarget* currentScreen = nullptr;
static std::vector< RenderTarget> createdRenderTargets;

const SpriteAtlas* Platform::LoadAtlas(const char* path) {
	std::string assetPath = assetDir + path;

	C2D_SpriteSheet sheet = C2D_SpriteSheetLoad(assetPath.data());

	if (sheet == nullptr) {
		EXCEPTION("Load atlas %s failed!", path);
	}

	int count = C2D_SpriteSheetCount(sheet);
	SpriteAtlas* atlas = new SpriteAtlas(count);

	for (int i = 0; i < count; ++i) {
		C2D_Image img = C2D_SpriteSheetGetImage(sheet, i);
		Sprite s;
		s.textureId = new C2D_Image(img);
		s.rect.position = { img.subtex->left * img.tex->width , img.subtex->top * img.tex->height };
		s.rect.size = { img.subtex->width, img.subtex->height };

		atlas->AddSprite(s);
	}

	return atlas;
}

Font Platform::LoadFont(const char* path) {

	std::string fontPath = assetDir + path;
	C2D_Font font = C2D_FontLoad(fontPath.data());
	if (font == nullptr)
		EXCEPTION("Load font %s failed!", path);

	return { font };
}

void Platform::DrawOnTexture(Texture texture) {

	if (texture == nullptr) {
		C2D_SceneBegin(currentScreen);
		return;
	}
	const C3D_Tex* tex = ((C2D_Image*)texture)->tex;

	for (const RenderTarget& rt : createdRenderTargets) {
		if (rt.tex == tex) {
			C2D_SceneBegin(rt.rt);
			return;
		}
	}

	EXCEPTION("Render target for texture 0x%X not found", texture);
}
void Platform::DrawOnScreen(ScreenId screen) {

	currentScreen = screens[(int)screen];
	C2D_SceneBegin(currentScreen);
}
void Platform::Draw(const Sprite& sprite, Rectangle dst, Color color, bool hFlip) {
	C2D_Image img = *sprite.GetTextureId<C2D_Image>();


	if (hFlip)
		dst.size.x *= -1;
	if (color != Colors::Black)
	{
		C2D_ImageTint tint = { 0 };
		u32 ucolor = C2D_Color32f(color.r, color.g, color.b, color.a);
		for (int i = 0; i < 4; ++i) {
			tint.corners[i].color = ucolor;
			tint.corners[i].blend = 1.0f;
		}
		C2D_DrawImageAt(img, dst.position.x, dst.position.y, 0, &tint, dst.size.x / (float)img.subtex->width, dst.size.y / (float)img.subtex->height);
	}
	else
	{
		C2D_DrawImageAt(img, dst.position.x, dst.position.y, 0, nullptr, dst.size.x / (float)img.subtex->width, dst.size.y / (float)img.subtex->height);
	}
	
}
void Platform::DrawText(const Font& font, Vector2Int position, const char* text, Color color, float scale) {
	C2D_Font f = (C2D_Font)font.fontId;

	C2D_Text t;
	C2D_TextBufClear(textBuffer);
	C2D_TextFontParse(&t, f, textBuffer, text);
	C2D_TextOptimize(&t);

	u32 c = C2D_Color32f(color.r, color.g, color.b, color.a);

	C2D_DrawText(&t, C2D_WithColor, position.x, position.y, 0, scale, scale, c);
}
void Platform::DrawLine(Vector2Int src, Vector2Int dst, Color color) {

	u32 c = C2D_Color32f(color.r, color.g, color.b, color.a);
	C2D_DrawLine(src.x, src.y, c, dst.x, dst.y, c, 1, 0);
}
void Platform::DrawRectangle(Rectangle rect, Color color) {
	u32 c = C2D_Color32f(color.r, color.g, color.b, color.a);
	C2D_DrawRectSolid(rect.position.x, rect.position.y, 0, rect.size.x, rect.size.y, c);
}
Texture Platform::NewTexture(Vector2Int size) {
	C3D_Tex* tex = new C3D_Tex();
	if (!C3D_TexInitVRAM(tex, size.x, size.y, GPU_TEXCOLOR::GPU_RGBA8))
		EXCEPTION("Failed to create texture with size %ix%i!", size.x, size.y);

	C3D_RenderTarget* rt = C3D_RenderTargetCreateFromTex(tex, GPU_TEXFACE::GPU_TEXFACE_2D, 0, -1);

	if (rt == nullptr) {
		EXCEPTION("Failed to create render target for texture with size %ix%i!", size.x, size.y);
	}

	C3D_RenderTargetClear(rt, C3D_ClearBits::C3D_CLEAR_COLOR, C2D_Color32(0, 0, 0, 255), 0);
	createdRenderTargets.push_back({ rt,tex });

	C2D_Image* result = new C2D_Image();
	result->tex = tex;
	Tex3DS_SubTexture* st = new Tex3DS_SubTexture();
	st->width = size.x;
	st->height = size.y;
	st->left = 0.0f;
	st->right = 0.0f;
	st->right = 1.0f;
	st->bottom = 1.0f;
	result->subtex = st;

	return result;
}

double Platform::ElaspedTime() {

	u64 now = svcGetSystemTick();

	TickCounter counter{ now - mainTimer, mainTimer };
	double elapsed = osTickCounterRead(&counter);

	return elapsed / 1000;
}

void Platform::UpdateGamepadState(GamepadState& state) {
	u32 kDown = hidKeysHeld();
	state.X = kDown & KEY_X;
	state.Y = kDown & KEY_Y;
	state.A = kDown & KEY_A;
	state.B = kDown & KEY_B;
	state.Select = kDown & KEY_SELECT;
	state.Start = kDown & KEY_START;
	state.DPad = Vector2Int(kDown & KEY_DRIGHT, kDown & KEY_DDOWN) - Vector2Int(kDown & KEY_DLEFT & kDown & KEY_DUP);
	state.L = kDown & KEY_L;
	state.R = kDown & KEY_R;
	state.ZL = kDown & KEY_ZL;
	state.ZR = kDown & KEY_ZR;


	circlePosition circlePos;
	hidCircleRead(&circlePos);

	state.CPad = Vector2(circlePos.dx, -circlePos.dy) / 154.0f;

	circlePosition cstickPos;
	hidCstickRead(&cstickPos);

	state.CStick = Vector2(cstickPos.dx, -cstickPos.dy) / 154.0f;
}
void Platform::UpdatePointerState(PointerState& state) {
	touchPosition touchPos;
	hidTouchRead(&touchPos);

	u32 kDown = hidKeysHeld();
	state.Touch = kDown && KEY_TOUCH;
	state.Position = { touchPos.px, touchPos.py };
}

FILE* Platform::OpenAsset(const char* path) {
	std::string f = assetDir + path;

	return fopen(f.data(), "rb");
}

void Platform::CreateChannel(AudioChannelState& channel) {
	if (audioChannels.size() == 24)
		EXCEPTION("Reached max number of channels!");

	int channelId = audioChannels.size();

	ndspChnReset(channelId);
	ndspChnSetInterp(channelId, NDSP_INTERP_LINEAR);
	ndspChnSetRate(channelId, 22050);
	ndspChnSetFormat(channelId, channel.mono ? NDSP_FORMAT_MONO_PCM16 : NDSP_FORMAT_STEREO_PCM16);
	float volume[12];
	for (int i = 0; i < 12; ++i)
		volume[i] = 0.8f;
	ndspChnSetMix(channelId, volume);
	NDSAudioChannel* platformChannel = new NDSAudioChannel();
	platformChannel->state = &channel;
	platformChannel->enabled = false;

	memset(&platformChannel->waveBuff[0], 0, sizeof(platformChannel->waveBuff));

	for (int i = 0; i < 2; ++i) {
		platformChannel->waveBuff[i].data_vaddr = linearAlloc(channel.bufferSize);
		platformChannel->waveBuff[i].nsamples = channel.bufferSize / (2* (channel.mono ? 1 : 2));
	}

	audioChannels.push_back(platformChannel);

	channel.handle = channelId;
}
void Platform::EnableChannel(const AudioChannelState& channel, bool enabled) {

	audioChannels[channel.handle]->enabled = enabled;
	ndspChnSetPaused(channel.handle, !enabled);
	if (!enabled)
	{
		ndspChnWaveBufClear(channel.handle);
	}
}
