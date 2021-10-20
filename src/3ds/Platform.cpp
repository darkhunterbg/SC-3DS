#include "..\game\Platform.h"
#include "Platform.h"

#include <citro3d.h>
#include <citro2d.h>
#include <3ds.h>
#include <string>
#include <vector>
#include <fstream>
#include <unordered_map>
#include <algorithm>

#include "StringLib.h"
#include "NDSAudioChannel.h"
#include "RenderTarget.h"

#include "Debug.h"
#include <cmath>


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

static std::unordered_map<const C3D_Tex*, const Tex3DS_SubTexture*> loadedTextures;


PlatformInfo Platform::GetPlatformInfo()
{
	PlatformInfo info;
	info.Type = PlatformType::Nintendo3DS;
	info.Screens.push_back(Vector2Int16(screens[0].size));
	info.Screens.push_back(Vector2Int16(screens[1].size));
	bool isNew3DS = false;
	APT_CheckNew3DS(&isNew3DS);
	if (isNew3DS)
		info.HardwareThreadsCount = 3;
	return info;
}

TextureId Platform::CreateTextureFromFile(const char* path, Span<uint8_t> data, Vector2Int16& outSize)
{
	std::string assetPath = assetDir + path;
	std::replace(assetPath.begin(), assetPath.end(), '\\', '/');


	C3D_Tex* tex = new	C3D_Tex();

	auto t3x = Tex3DS_TextureImport(data.Data(), data.Size(), tex, nullptr, false);

	loadedTextures[tex] = Tex3DS_GetSubTexture(t3x, 0);

	C3D_TexSetWrap(tex, GPU_TEXTURE_WRAP_PARAM::GPU_CLAMP_TO_EDGE, GPU_TEXTURE_WRAP_PARAM::GPU_CLAMP_TO_EDGE);
	C3D_TexSetFilter(tex, GPU_TEXTURE_FILTER_PARAM::GPU_LINEAR, GPU_TEXTURE_FILTER_PARAM::GPU_LINEAR);

	outSize.x = loadedTextures[tex]->width;
	outSize.y = loadedTextures[tex]->height;

	return tex;
}

//TextureId Platform::LoadTexture(const char* path, Vector2Int16& outSize)
//{
//	std::string assetPath = assetDir + path + ".t3x";
//	std::replace(assetPath.begin(), assetPath.end(), '\\', '/');
//	FILE* f = fopen(assetPath.data(), "rb");
//	if (f == nullptr)
//		EXCEPTION("Failed to open file '%s'", assetPath.data());
//
//	setvbuf(f, NULL, _IOFBF, 64 * 1024);
//
//	C3D_Tex* tex = new	C3D_Tex();
//
//	auto t3x = Tex3DS_TextureImportStdio(f, tex, nullptr, false);
//
//	loadedTextures[tex] = Tex3DS_GetSubTexture(t3x, 0);
//
//	C3D_TexSetWrap(tex, GPU_TEXTURE_WRAP_PARAM::GPU_CLAMP_TO_EDGE, GPU_TEXTURE_WRAP_PARAM::GPU_CLAMP_TO_EDGE);
//	C3D_TexSetFilter(tex, GPU_TEXTURE_FILTER_PARAM::GPU_LINEAR, GPU_TEXTURE_FILTER_PARAM::GPU_LINEAR);
//
//	outSize.x = loadedTextures[tex]->width;
//	outSize.y = loadedTextures[tex]->height;
//
//	return tex;
//}

const Font* Platform::LoadFont(const char* path, int size)
{

	std::string fontPath = assetDir + path;

	if (loadedFonts.find(fontPath) == loadedFonts.end())
	{

		C2D_Font font = C2D_FontLoad(fontPath.data());
		if (font == nullptr)
			EXCEPTION("Load font %s failed!", path);

		loadedFonts[fontPath] = font;
	}
	float scale = (float)size / 30.0f;
	return new Font(loadedFonts[fontPath], scale);
}
Vector2Int Platform::MeasureString(const Font& font, const char* text)
{
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
void Platform::ChangeBlendingMode(BlendMode mode)
{

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

void Platform::DrawOnSurface(SurfaceId surface)
{

	if (surface == nullptr)
	{
		currentRT = screens[currentScreen];
		C2D_SceneBegin(currentRT.rt);
		return;
	}
	C3D_RenderTarget* rt = (C3D_RenderTarget*)surface;
	C2D_SceneBegin(rt);

	for (auto& crt : createdRenderTargets)
	{
		if (crt.rt == rt)
		{
			currentRT = crt;
			return;
		}
	}

	EXCEPTION("RenderTarget not found!");
}

SurfaceId Platform::NewRenderSurface(Vector2Int size, bool pixelFiltering, TextureId& outTexture)
{
	C3D_Tex* tex = new C3D_Tex();
	if (!C3D_TexInitVRAM(tex, size.x, size.y, GPU_TEXCOLOR::GPU_RGBA8))
		EXCEPTION("Failed to create texture with size %ix%i!", size.x, size.y);

	C3D_RenderTarget* rt = C3D_RenderTargetCreateFromTex(tex, GPU_TEXFACE::GPU_TEXFACE_2D, 0, -1);

	outTexture = tex;

	if (rt == nullptr)
	{
		EXCEPTION("Failed to create render target for texture with size %ix%i!", size.x, size.y);
	}

	C3D_RenderTargetClear(rt, C3D_ClearBits::C3D_CLEAR_COLOR, C2D_Color32(0, 0, 0, 255), 0);

	RenderTarget renderTarget = { rt,tex, size };
	renderTarget.Init();

	createdRenderTargets.push_back(renderTarget);

	Tex3DS_SubTexture* sub = new Tex3DS_SubTexture();
	sub->width = tex->width;
	sub->height = tex->height;
	sub->top = 1;
	sub->left = 0;
	sub->bottom = 0;
	sub->right = 1;

	loadedTextures[tex] = sub;

	if (!pixelFiltering)
		C3D_TexSetFilter(tex, GPU_TEXTURE_FILTER_PARAM::GPU_LINEAR, GPU_TEXTURE_FILTER_PARAM::GPU_LINEAR);
	else
		C3D_TexSetFilter(tex, GPU_TEXTURE_FILTER_PARAM::GPU_NEAREST, GPU_TEXTURE_FILTER_PARAM::GPU_NEAREST);

	return rt;
}

void Platform::DrawOnScreen(ScreenId screen)
{

	currentScreen = (int)screen;
	currentRT = screens[currentScreen];
	C2D_SceneBegin(currentRT.rt);
}

void Platform::ClearBuffer(Color color)
{
	C2D_TargetClear(currentRT.rt, Color32(color).value);
}

SubImageCoord Platform::GenerateUV(TextureId texture, Rectangle16 src)
{
	C3D_Tex* tex = (C3D_Tex*)texture;

	if (loadedTextures.find(tex) == loadedTextures.end())
		EXCEPTION("Invalid texture for UV generation!");

	const Tex3DS_SubTexture* subTex = loadedTextures[tex];

	Vector2Int size = { subTex->width, subTex->height };
	Vector2 min = Vector2(src.GetMin()) / Vector2(size);
	Vector2 max = Vector2(src.GetMax()) / Vector2(size);

	Tex3DS_SubTexture st2 = *subTex;
	st2.width = src.size.x;
	st2.height = src.size.y;

	st2.left = LerpF(subTex->left, subTex->right, min.x);
	st2.top = LerpF(subTex->top, subTex->bottom, min.y);
	st2.right = LerpF(subTex->left, subTex->right, max.x);
	st2.bottom = LerpF(subTex->top, subTex->bottom, max.y);

	SubImageCoord uv = { };

	Tex3DS_SubTextureTopLeft(&st2, &uv.coords[0].x, &uv.coords[0].y);
	Tex3DS_SubTextureTopRight(&st2, &uv.coords[1].x, &uv.coords[1].y);
	Tex3DS_SubTextureBottomLeft(&st2, &uv.coords[2].x, &uv.coords[2].y);
	Tex3DS_SubTextureBottomRight(&st2, &uv.coords[3].x, &uv.coords[3].y);

	return uv;
}

void Platform::DrawTexture(const Texture& texture, const SubImageCoord& uv, const Rectangle16& dst, bool hFlip, Color32 c)
{
	Vector2 uvSize = uv.corners.topRight - uv.corners.bottomLeft;

	Vector2Int16 size = Vector2Int16(Vector2(texture.GetSize()) * uvSize);

	Vector2 scale = Vector2(dst.size) / Vector2(size);
	Vector2 s = scale;
	scale.x -= scale.x * 2.0f * hFlip;

	C2D_Image img;
	img.tex = (C3D_Tex*)texture.GetTextureId();
	auto subTex = loadedTextures[img.tex];

	auto st2 = *subTex;

	st2.width = size.x;
	st2.height = size.y;

	st2.left = uv.corners.topLeft.x;
	st2.top = uv.corners.topLeft.y;
	st2.right = uv.corners.bottomRight.x;
	st2.bottom = uv.corners.bottomRight.y;

	img.subtex = &st2;

	C2D_ImageTint tint;

	C2D_PlainImageTint(&tint, c.value, 1);

	C2D_DrawImageAt(img, (float)dst.position.x, (float)dst.position.y, 0, &tint, scale.x, scale.y);
}
void Platform::DrawRectangle(const Rectangle& rect, Color32 c)
{
	C2D_DrawRectSolid((float)rect.position.x, (float)rect.position.y, 0, (float)rect.size.x, (float)rect.size.y, c.value);
}

void Platform::DrawText(const Font& font, Vector2Int position, const char* text, Color color)
{
	C2D_Font f = (C2D_Font)font.GetFontIdRaw();

	C2D_Text t;
	C2D_TextBufClear(textBuffer);
	C2D_TextFontParse(&t, f, textBuffer, text);
	C2D_TextOptimize(&t);

	u32 c = C2D_Color32f(color.r, color.g, color.b, color.a);

	C2D_DrawText(&t, C2D_WithColor, position.x, position.y, 0, font.GetScale(), font.GetScale(), c);
}


double Platform::ElaspedTime()
{

	u64 now = svcGetSystemTick();

	TickCounter counter{ now - mainTimer, mainTimer };
	double elapsed = osTickCounterRead(&counter);

	return elapsed / 1000;
}

void Platform::UpdateGamepadState(GamepadState& state)
{
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
void Platform::UpdatePointerState(PointerState& state)
{
	touchPosition touchPos;
	hidTouchRead(&touchPos);

	u32 kDown = hidKeysHeld();
	state.Touch = kDown && KEY_TOUCH;
	state.Position = { touchPos.px, touchPos.py };
}

FILE* Platform::OpenAsset(const char* path, AssetType type)
{
	std::string f = assetDir + path;
	std::replace(f.begin(), f.end(), '\\', '/');

	switch (type)
	{
	case AssetType::Unknown:
		break;
	case AssetType::Texture:
		f += ".t3x"; break;
	case AssetType::Font:
		f += ".bcfnt"; break;
	case AssetType::AudioClip:
		f += ".wav"; break;
	case AssetType::Database:
		f += ".bin"; break;
		break;
	default:
		break;
	}

	return fopen(f.data(), "rb");
}

void Platform::CreateChannel(AudioChannelState& channel)
{
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

	for (int i = 0; i < 2; ++i)
	{
		platformChannel->waveBuff[i].data_vaddr = linearAlloc(channel.bufferSize);
		platformChannel->waveBuff[i].nsamples = channel.bufferSize / (2 * (channel.mono ? 1 : 2));
	}

	audioChannels.push_back(platformChannel);

	channel.handle = channelId;
}
void Platform::EnableChannel(const AudioChannelState& channel, bool enabled)
{

	audioChannels[channel.handle]->enabled = enabled;
	ndspChnSetPaused(channel.handle, !enabled);
	if (!enabled)
	{
		ndspChnWaveBufClear(channel.handle);
	}
}

static int threadIds[] = { 1,2,3 };

static std::function<void(int)> f;

static void ThreadWork(void* arg)
{
	f(*(int*)arg);
}

int Platform::TryStartThreads(ThreadUsageType type, int requestCount, std::function<void(int)> threadWork)
{
	f = threadWork;

	int threads = 0;
	std::vector<int> cores;

	s32 prio = 0;

	Result r = svcGetThreadPriority(&prio, CUR_THREAD_HANDLE);
	if (r)
		EXCEPTION("svcGetThreadPriority failed with %s", R_SUMMARY(r));

	if (type == ThreadUsageType::JobSystem)
	{
		for (int i = 0; i < requestCount; ++i)
		{
			if (threadCreate(ThreadWork, &threadIds[threads], 4096, prio - 1, threadIds[i], true))
			{
				cores.push_back(threadIds[i]);
				++threads;
			}
		}
	}
	else if (type == ThreadUsageType::IO)
	{
		if (threadCreate(ThreadWork, new int(100), 4096, prio - 2, -2, true))
		{
			cores.push_back(0);
			++threads;
		}
		else
		{
			EXCEPTION("Failed to create IO thread!");
		}
	}

	//std::string coreIds;
	//for (int i : cores)
	//	coreIds += std::to_string(i) + ",";

	//EXCEPTION("Created threads on cores %s, %i threads", coreIds.data(), cores.size());


	return threads;
}
static int semaphoreCount;

Semaphore Platform::CreateSemaphore()
{
	Handle* h = new Handle();
	Result r = svcCreateSemaphore(h, 0, 100000);
	if (r)
		EXCEPTION("svcCreateSemaphore failed with %i: %s", r, R_SUMMARY(r));


	return h;
}
void Platform::WaitSemaphore(Semaphore s)
{
	GAME_ASSERT((Handle*)s, "Tried to wait for null semaphore!");

	Result r = svcWaitSynchronization(*(Handle*)s, -1);
	if (r)
		EXCEPTION("svcWaitSynchronization failed with %i: %s", r, R_SUMMARY(r));
}
void Platform::ReleaseSemaphore(Semaphore s, int v)
{
	GAME_ASSERT((Handle*)s, "Tried to release null semaphore!");

	s32 o;
	Result r = svcReleaseSemaphore(&o, *(Handle*)s, v);
	if (r)
		EXCEPTION("svcReleaseSemaphore failed with %s", R_SUMMARY(r));
}

std::string Platform::GetUserDirectory()
{
	return  userDir;
}
