#include "..\game\Platform.h"
#include "Platform.h"

#include <citro3d.h>
#include <citro2d.h>
#include <3ds.h>
#include <string>
#include <vector>
#include <fstream>
#include <unordered_map>

#include "StringLib.h"
#include "NDSAudioChannel.h"
#include "RenderTarget.h"

#include "Debug.h"


extern RenderTarget screens[2];
extern std::string assetDir;
extern std::string userDir;
extern C2D_TextBuf textBuffer;
extern u64 mainTimer;
extern std::vector<NDSAudioChannel*> audioChannels;
static RenderTarget currentRT;
static std::vector<RenderTarget> createdRenderTargets;
static int currentScreen = 0;
static std::unordered_map<std::string, C2D_Font> loadedFonts;
static bool c2d_Prepared = true;

extern Span<Vertex> vertexBuffer;
extern C3D_BufInfo vbInfo;

extern DVLB_s* spriteBatchBlob;
extern shaderProgram_s spriteBatchProgram;
extern C3D_AttrInfo spriteBatchAttributeInfo;


TextureId Platform::LoadTexture(const char* path, Vector2Int16& outSize) {
	std::string assetPath = assetDir + path + ".t3x";
	FILE* f = fopen(assetPath.data(),"rb");
	if (f == nullptr)
		EXCEPTION("Failed to open file '%s'", assetPath.data());
	C3D_Tex* tex = new	C3D_Tex();

	auto t3x = Tex3DS_TextureImportStdio(f, tex, nullptr, false);

	C3D_TexSetWrap(tex, GPU_TEXTURE_WRAP_PARAM::GPU_CLAMP_TO_EDGE, GPU_TEXTURE_WRAP_PARAM::GPU_CLAMP_TO_EDGE);
	C3D_TexSetFilter(tex, GPU_TEXTURE_FILTER_PARAM::GPU_LINEAR, GPU_TEXTURE_FILTER_PARAM::GPU_LINEAR);

	outSize.x = tex->width;
	outSize.y = tex->height;

	return tex;
}


/*
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
		s.textureId = img.tex;

		Tex3DS_SubTextureTopLeft(img.subtex, &s.uv[0].x, &s.uv[0].y);
		Tex3DS_SubTextureTopRight(img.subtex, &s.uv[1].x, &s.uv[1].y);
		Tex3DS_SubTextureBottomLeft(img.subtex, &s.uv[2].x, &s.uv[2].y);
		Tex3DS_SubTextureBottomRight(img.subtex, &s.uv[3].x, &s.uv[3].y);

		s.rect.size = { (short)img.subtex->width, (short)img.subtex->height };
		s.rect.position = { img.subtex->left * img.tex->width , img.subtex->top * img.tex->height };

		atlas->AddSprite(s);
	}
	C2D_Image img = C2D_SpriteSheetGetImage(sheet, 0);
	C3D_TexSetWrap(img.tex, GPU_TEXTURE_WRAP_PARAM::GPU_CLAMP_TO_EDGE, GPU_TEXTURE_WRAP_PARAM::GPU_CLAMP_TO_EDGE);
	C3D_TexSetFilter(img.tex, GPU_TEXTURE_FILTER_PARAM::GPU_LINEAR, GPU_TEXTURE_FILTER_PARAM::GPU_LINEAR);

	return atlas;
}*/
const Font* Platform::LoadFont(const char* path, int size) {

	std::string fontPath = assetDir + path;

	if (loadedFonts.find(fontPath) == loadedFonts.end()) {

		C2D_Font font = C2D_FontLoad(fontPath.data());
		if (font == nullptr)
			EXCEPTION("Load font %s failed!", path);

		loadedFonts[fontPath] = font;
	}
	float scale = (float)size / 30.0f;
	return new Font(loadedFonts[fontPath], scale);
}
Vector2Int Platform::MeasureString(const Font& font, const char* text) {
	C2D_Font f = (C2D_Font)font.GetFontIdRaw();

	C2D_TextBufClear(textBuffer);
	C2D_Text t;
	C2D_TextFontParse(&t, f, textBuffer, text);
	C2D_TextOptimize(&t);

	Vector2Int size;

	size.x = (int)(t.width * font.GetScale());
	size.y = (int)(t.lines * 30.0f * font.GetScale());

	//EXCEPTION("%s : %i %i",text, size.x, size.y);

	return size;
}
void Platform::ChangeBlendingMode(BlendMode mode) {

	C2D_Flush();

	switch (mode)
	{
	case BlendMode::Alpha:
		C3D_AlphaBlend(
			GPU_BLENDEQUATION::GPU_BLEND_ADD, GPU_BLENDEQUATION::GPU_BLEND_ADD,
			GPU_BLENDFACTOR::GPU_SRC_ALPHA, GPU_BLENDFACTOR::GPU_ONE_MINUS_SRC_ALPHA,
			GPU_BLENDFACTOR::GPU_SRC_ALPHA, GPU_BLENDFACTOR::GPU_ONE_MINUS_SRC_ALPHA);
		break;
	case BlendMode::AlphaSet:
		C3D_AlphaBlend(
			GPU_BLENDEQUATION::GPU_BLEND_ADD, GPU_BLENDEQUATION::GPU_BLEND_ADD,
			GPU_BLENDFACTOR::GPU_SRC_ALPHA, GPU_BLENDFACTOR::GPU_ZERO,
			GPU_BLENDFACTOR::GPU_SRC_ALPHA, GPU_BLENDFACTOR::GPU_ZERO);
		break;
	case BlendMode::AllSet:
		C3D_AlphaBlend(
			GPU_BLENDEQUATION::GPU_BLEND_ADD, GPU_BLENDEQUATION::GPU_BLEND_ADD,
			GPU_BLENDFACTOR::GPU_ONE, GPU_BLENDFACTOR::GPU_ZERO,
			GPU_BLENDFACTOR::GPU_ONE, GPU_BLENDFACTOR::GPU_ZERO);
		break;
	default:
		break;
	}
}

void Platform::DrawOnSurface(SurfaceId surface) {

	if (surface == nullptr) {
		currentRT = screens[currentScreen];
		C2D_SceneBegin(currentRT.rt);
		return;
	}
	C3D_RenderTarget* rt = (C3D_RenderTarget*)surface;
	C2D_SceneBegin(rt);

	for (auto& crt : createdRenderTargets) {
		if (crt.rt == rt) {
			currentRT = crt;
			return;
		}
	}

	EXCEPTION("RenderTarget not found!");
}


SurfaceId Platform::NewRenderSurface(Vector2Int size, bool pixelFiltering, TextureId& outTexture) {
	C3D_Tex* tex = new C3D_Tex();
	if (!C3D_TexInitVRAM(tex, size.x, size.y, GPU_TEXCOLOR::GPU_RGBA8))
		EXCEPTION("Failed to create texture with size %ix%i!", size.x, size.y);

	C3D_RenderTarget* rt = C3D_RenderTargetCreateFromTex(tex, GPU_TEXFACE::GPU_TEXFACE_2D, 0, -1);

	outTexture = tex;

	if (rt == nullptr) {
		EXCEPTION("Failed to create render target for texture with size %ix%i!", size.x, size.y);
	}

	C3D_RenderTargetClear(rt, C3D_ClearBits::C3D_CLEAR_COLOR, C2D_Color32(0, 0, 0, 255), 0);

	RenderTarget renderTarget = { rt,tex, size };
	renderTarget.Init();

	createdRenderTargets.push_back(renderTarget);

	if (!pixelFiltering)
		C3D_TexSetFilter(tex, GPU_TEXTURE_FILTER_PARAM::GPU_LINEAR, GPU_TEXTURE_FILTER_PARAM::GPU_LINEAR);
	else
		C3D_TexSetFilter(tex, GPU_TEXTURE_FILTER_PARAM::GPU_NEAREST, GPU_TEXTURE_FILTER_PARAM::GPU_NEAREST);

	//Sprite s;
	//s.rect = { {0,0}, Vector2Int16(size) };
	//s.uv[0] = { 0,1 };
	//s.uv[1] = { 1,1 };
	//s.uv[2] = { 0,0 };
	//s.uv[3] = { 1,0 };
	//s.textureId = tex;


	return rt;
}

void Platform::DrawOnScreen(ScreenId screen) {

	currentScreen = (int)screen;
	currentRT = screens[currentScreen];
	C2D_SceneBegin(currentRT.rt);
}

void Platform::ClearBuffer(Color color) {
	C2D_TargetClear(currentRT.rt, Color32(color).value);
}

SubImageCoord Platform::GenerateUV(TextureId texture, Rectangle16 src) {
	C3D_Tex* tex = (C3D_Tex*)texture;

	Vector2Int size = { tex->width, tex->height };

	Vector2 min = {
	(float)src.position.x / (float)size.x ,
	1.0f - (float)src.position.y / (float)size.y };
	Vector2 max = {
		(float)(src.position.x + src.size.x) / (float)size.x,
		1 - (float)(src.position.y + src.size.y) / (float)size.y
	};

	return { min, {max.x, min.y}, {min.x, max.y}, max };
}
//
//Sprite Platform::NewSprite(Texture texture, Rectangle16 src) {
//	C3D_Tex* tex = (C3D_Tex*)texture;
//
//	Vector2Int size = { tex->width, tex->height };
//
//	Vector2 min = { 
//		(float)src.position.x / (float)size.x ,
//		1.0f - (float)src.position.y / (float)size.y };
//	Vector2 max = { 
//		(float)(src.position.x + src.size.x) / (float)size.x,
//		1 - (float)(src.position.y + src.size.y) / (float)size.y 
//	};
//
//	return Sprite{
//		src, 
//		{ min, {max.x, min.y}, {min.x, max.y}, max },
//		tex 
//	};
//}

Span<Vertex> Platform::GetVertexBuffer() {

	return  vertexBuffer;
}

static void SetEnv(DrawCommandType type) {
	C3D_TexEnv* env = C3D_GetTexEnv(0);
	C3D_TexEnvInit(env);
	switch (type)
	{
	case DrawCommandType::TexturedTriangle:
		C3D_TexEnvFunc(env, C3D_Both, GPU_MODULATE);
		C3D_TexEnvSrc(env, C3D_Both, GPU_TEXTURE0, GPU_PRIMARY_COLOR, GPU_PRIMARY_COLOR);

		break;
	case DrawCommandType::Triangle:
		C3D_TexEnvFunc(env, C3D_Both, GPU_REPLACE);
		C3D_TexEnvSrc(env, C3D_Both, GPU_PRIMARY_COLOR, GPU_PRIMARY_COLOR, GPU_PRIMARY_COLOR);
		break;
	default:
		break;
	}
}

void Platform::ExecDrawCommands(const Span<DrawCommand> commands) {
	
	if (c2d_Prepared) {
		C2D_Flush();

		C3D_TexEnv* env = C3D_GetTexEnv(1);
		C3D_TexEnvInit(env);

		C3D_BindProgram(&spriteBatchProgram);
		C3D_SetAttrInfo(&spriteBatchAttributeInfo);
		C3D_SetBufInfo(&vbInfo);

		C3D_DepthTest(true, GPU_GEQUAL, GPU_WRITE_ALL);
		C3D_CullFace(GPU_CULL_NONE);

		c2d_Prepared = false;

		C3D_Mtx mdlvMtx;
		Mtx_Identity(&mdlvMtx);
		auto uLoc_mdlvMtx = shaderInstanceGetUniformLocation(spriteBatchProgram.vertexShader, "mdlvMtx");
		C3D_FVUnifMtx4x4(GPU_VERTEX_SHADER, uLoc_mdlvMtx, &mdlvMtx);
	}


	auto uLoc_projMtx = shaderInstanceGetUniformLocation(spriteBatchProgram.vertexShader, "projMtx");

	C3D_FVUnifMtx4x4(GPU_VERTEX_SHADER, uLoc_projMtx, &currentRT.projMtx);

	SetEnv(commands[0].type);
	DrawCommandType type = commands[0].type;

	for (const DrawCommand& cmd : commands)
	{
		if (cmd.type != type) {
			type = cmd.type;
			SetEnv(cmd.type);
		}

		switch (cmd.type)
		{
		case DrawCommandType::TexturedTriangle: {
			C3D_TexBind(0, (C3D_Tex*)cmd.texture);
			C3D_DrawArrays(GPU_TRIANGLES, cmd.start, cmd.count);
			break;
		}
		case DrawCommandType::Triangle: {
			C3D_DrawArrays(GPU_TRIANGLES, cmd.start, cmd.count);
			break;
		}
		default:
			break;
		}
	}


}

void Platform::DrawText(const Font& font, Vector2Int position, const char* text, Color color) {

	if (!c2d_Prepared) {
		C2D_Prepare();
		c2d_Prepared = true;
	}


	C2D_Font f = (C2D_Font)font.GetFontIdRaw();

	C2D_Text t;
	C2D_TextBufClear(textBuffer);
	C2D_TextFontParse(&t, f, textBuffer, text);
	C2D_TextOptimize(&t);

	u32 c = C2D_Color32f(color.r, color.g, color.b, color.a);

	C2D_DrawText(&t, C2D_WithColor, position.x, position.y, 0, font.GetScale(), font.GetScale(), c);
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
		volume[i] = channel.volume * 0.8f;
	ndspChnSetMix(channelId, volume);
	NDSAudioChannel* platformChannel = new NDSAudioChannel();
	platformChannel->id = channelId;
	platformChannel->state = &channel;
	platformChannel->enabled = false;

	memset(&platformChannel->waveBuff[0], 0, sizeof(platformChannel->waveBuff));

	for (int i = 0; i < 2; ++i) {
		platformChannel->waveBuff[i].data_vaddr = linearAlloc(channel.bufferSize);
		platformChannel->waveBuff[i].nsamples = channel.bufferSize / (2 * (channel.mono ? 1 : 2));
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

static  int threadIds[] = { 1,2,3 };

static std::function<void(int)> f;

static void ThreadWork(void* arg) {
	f(*(int*)arg);
}

int Platform::StartThreads(std::function<void(int)> threadWork) {

	f = threadWork;

	int threads = 0;

	s32 prio = 0;

	Result r = svcGetThreadPriority(&prio, CUR_THREAD_HANDLE);
	if (r)
		EXCEPTION("svcGetThreadPriority failed with %s", R_SUMMARY(r));

	std::vector<int> cores;

	for (int i = 0; i < 3; ++i) {
		if (threadCreate(ThreadWork, &threadIds[threads], 4096, prio - 1, threadIds[i], true)) {
			cores.push_back(threadIds[i]);
			++threads;
		}
	}

	//std::string coreIds;
	//for (int i : cores)
	//	coreIds += std::to_string(i) + ",";

	//EXCEPTION("Created threads on cores %s, %i threads", coreIds.data(), cores.size());

	return threads;
}
Semaphore Platform::CreateSemaphore() {
	Handle* h = new Handle(0);
	Result r = svcCreateSemaphore(h, 0, 100000);
	if (r)
		EXCEPTION("svcCreateSemaphore failed with %s", R_SUMMARY(r));

	return h;
}
void Platform::WaitSemaphore(Semaphore s) {
	Result r = svcWaitSynchronization(*(Handle*)s, -1);
	if (r)
		EXCEPTION("svcWaitSynchronization failed with %s", R_SUMMARY(r));
}
void Platform::ReleaseSemaphore(Semaphore s, int v) {
	s32 o;
	Result r = svcReleaseSemaphore(&o, *(Handle*)s, v);
	if (r)
		EXCEPTION("svcReleaseSemaphore failed with %s", R_SUMMARY(r));
}

std::string Platform::GetUserDirectory()
{
	return  userDir;
}
