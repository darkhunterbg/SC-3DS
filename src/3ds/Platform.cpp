#include "..\game\Platform.h"
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

#include "Debug.h"

struct RenderTarget {
	C3D_RenderTarget* rt;
	C3D_Tex* tex;
};


extern C3D_RenderTarget* screens[2];
extern std::string assetDir;
extern std::string userDir;
extern C2D_TextBuf textBuffer;
extern u64 mainTimer;
extern std::vector<NDSAudioChannel*> audioChannels;
static C3D_RenderTarget* currentRT = nullptr;
static std::vector< RenderTarget> createdRenderTargets;
static int currentScreen = 0;


extern Vertex* vertexBuffer;
extern C3D_BufInfo vbInfo;

extern DVLB_s* spriteBatchBlob;
extern shaderProgram_s spriteBatchProgram;
extern C3D_AttrInfo spriteBatchAttributeInfo;

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
		s.image.textureId = img.tex;
		s.uv[0] = { img.subtex->left, img.subtex->top };
		s.uv[1] = { img.subtex->right, img.subtex->bottom };
			// (Texture)img.subtex;
		//s.rect.position = { img.subtex->left * img.tex->width , img.subtex->top * img.tex->height };
		s.rect.size = { img.subtex->width, img.subtex->height };

		atlas->AddSprite(s);
	}
	C2D_Image img = C2D_SpriteSheetGetImage(sheet, 0);
	C3D_TexSetWrap(img.tex, GPU_TEXTURE_WRAP_PARAM::GPU_CLAMP_TO_EDGE, GPU_TEXTURE_WRAP_PARAM::GPU_CLAMP_TO_EDGE);
	C3D_TexSetFilter(img.tex, GPU_TEXTURE_FILTER_PARAM::GPU_LINEAR, GPU_TEXTURE_FILTER_PARAM::GPU_LINEAR);

	return atlas;
}

Font Platform::LoadFont(const char* path) {

	std::string fontPath = assetDir + path;
	C2D_Font font = C2D_FontLoad(fontPath.data());
	if (font == nullptr)
		EXCEPTION("Load font %s failed!", path);

	return { font };
}
Vector2Int Platform::MeasureString(Font font, const char* text, float scale) {
	C2D_Font f = (C2D_Font)font.fontId;

	C2D_TextBufClear(textBuffer);
	C2D_Text t;
	C2D_TextFontParse(&t, f, textBuffer, text);
	C2D_TextOptimize(&t);

	Vector2Int size;

	size.x = (int)(t.width * scale);
	size.y = (int)(t.lines * 30.0f * scale);

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
	case BlendMode::AlphaOverride:
		C3D_AlphaBlend(
			GPU_BLENDEQUATION::GPU_BLEND_ADD, GPU_BLENDEQUATION::GPU_BLEND_ADD,
			GPU_BLENDFACTOR::GPU_SRC_ALPHA, GPU_BLENDFACTOR::GPU_ZERO,
			GPU_BLENDFACTOR::GPU_SRC_ALPHA, GPU_BLENDFACTOR::GPU_ZERO);
		break;
	case BlendMode::FullOverride:
		C3D_AlphaBlend(
			GPU_BLENDEQUATION::GPU_BLEND_ADD, GPU_BLENDEQUATION::GPU_BLEND_ADD,
			GPU_BLENDFACTOR::GPU_ONE, GPU_BLENDFACTOR::GPU_ZERO,
			GPU_BLENDFACTOR::GPU_ONE, GPU_BLENDFACTOR::GPU_ZERO);
		break;
	default:
		break;
	}
}

// Render without blending

void Platform::DrawOnTexture(Texture texture) {

	if (texture == nullptr) {
		currentRT = screens[currentScreen];
		C2D_SceneBegin(currentRT);
		return;
	}
	const C3D_Tex* tex = (C3D_Tex*)texture;

	for (const RenderTarget& rt : createdRenderTargets) {
		if (rt.tex == tex) {
			currentRT = rt.rt;

			C2D_SceneBegin(rt.rt);
			return;
		}
	}

	EXCEPTION("Render target for texture 0x%X not found", texture);
}
void Platform::DrawOnScreen(ScreenId screen) {

	currentScreen = (int)screen;
	currentRT = screens[currentScreen];
	C2D_SceneBegin(currentRT);
}

void Platform::ClearBuffer(Color color) {
	C2D_TargetClear(currentRT, Color32(color).value);
}

Sprite Platform::NewSprite(Image image, Rectangle16 src) {
	C3D_Tex* tex = (C3D_Tex*)image.textureId;

	//auto* s = (Tex3DS_SubTexture*)image.textureId2;

	auto* s2 = new Tex3DS_SubTexture();


	//s2->width = src.size.x;
	//s2->height = src.size.y;
	//s2->top = 1 - (float)src.position.y / (float)s->height;
	//s2->bottom = 1 - (float)(src.position.y + src.size.y) / (float)s->height;
	//s2->left = (float)src.position.x / (float)s->width;
	//s2->right = (float)(src.position.x + src.size.x) / (float)s->width;

	//EXCEPTION("%.2f %.2f", s2->left, s2->right);

	return { src, {tex} };
}

//void Platform::BatchDraw(const Span<BatchDrawCommand> commands) {
//
//	static constexpr const int depthStep = 0.0001f;
//
//	for (const auto& cmd : commands) {
//		C2D_Image img = *(C2D_Image*)&cmd.image;
//
//		C2D_ImageTint tint;
//		tint.corners[0] = { cmd.color.value, 1};
//		tint.corners[1] = { cmd.color.value, 1 };
//		tint.corners[2] = { cmd.color.value, 1};
//		tint.corners[3] = { cmd.color.value,1};
//		C2D_DrawImageAt(img, cmd.position.x, cmd.position.y, 0, &tint, cmd.scale.x, cmd.scale.y);
//	}
//}


Vertex* Platform::GetVertexBuffer() {

	return  vertexBuffer;
}
void Platform::ExecDrawCommands(const Span<DrawCommand> commands) {

	auto uLoc_mdlvMtx = shaderInstanceGetUniformLocation(spriteBatchProgram.vertexShader, "mdlvMtx");
	auto uLoc_projMtx = shaderInstanceGetUniformLocation(spriteBatchProgram.vertexShader, "projMtx");
	C3D_Mtx projMtx, mdlvMtx;

	Mtx_OrthoTilt(&projMtx, 0.0f, 400, 240, 0.0f, 1.0f, -1.0f, true);
	Mtx_Identity(&mdlvMtx);


	C3D_TexEnv* env = C3D_GetTexEnv(0);
	C3D_TexEnvInit(env);
	//C3D_TexEnvSrc set afterwards by C2Di_Update()
	C3D_TexEnvFunc(env, C3D_Both, GPU_MODULATE);
	C3D_TexEnvSrc(env, C3D_Both, GPU_PRIMARY_COLOR, GPU_TEXTURE0, 0);

	env = C3D_GetTexEnv(1);
	C3D_TexEnvInit(env);

	//C3D_TexEnvSrc(env, C3D_Both, GPU_PREVIOUS, GPU_PRIMARY_COLOR, 0);
	//C3D_TexEnvFunc(env, C3D_Both, GPU_MODULATE);

	C3D_BindProgram(&spriteBatchProgram);
	C3D_SetAttrInfo(&spriteBatchAttributeInfo);
	C3D_SetBufInfo(&vbInfo);

	C3D_FVUnifMtx4x4(GPU_VERTEX_SHADER, uLoc_projMtx, &projMtx);
	C3D_FVUnifMtx4x4(GPU_VERTEX_SHADER, uLoc_mdlvMtx, &mdlvMtx);

	C3D_DepthTest(true, GPU_GEQUAL, GPU_WRITE_ALL);
	C3D_CullFace(GPU_CULL_NONE);


	for (const DrawCommand& cmd : commands)
	{
		C3D_Tex* img = (C3D_Tex*)cmd.texture;

		C3D_TexBind(0, img);
		/*switch (cmd.type)
		{
		case DrawPrimitiveType::Triangle: {*/
		C3D_DrawArrays(GPU_TRIANGLES, cmd.start, cmd.count);
	
	}

	C2D_Prepare();
}

void Platform::Draw(const Sprite& sprite, Rectangle dst, Color color, bool hFlip, bool vFlip) {
	C2D_Image img = *(C2D_Image*)&sprite.image;

	if (hFlip)
		dst.size.x *= -1;
	if (vFlip)
		dst.size.y *= -1;
	//if (color != Colors::Black)
	//{
	C2D_ImageTint tint;
	u32 ucolor = C2D_Color32f(color.r, color.g, color.b, color.a);
	tint.corners[0] = { ucolor , 1.0f };
	tint.corners[1] = { ucolor , 1.0f };
	tint.corners[2] = { ucolor , 1.0f };
	tint.corners[3] = { ucolor , 1.0f };

	C2D_DrawImageAt(img, dst.position.x, dst.position.y, 0, &tint, dst.size.x / (float)img.subtex->width, dst.size.y / (float)img.subtex->height);
	//}
	//else
	//	C2D_DrawImageAt(img, dst.position.x, dst.position.y, 0, nullptr, dst.size.x / (float)img.subtex->width, dst.size.y / (float)img.subtex->height);


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
void Platform::DrawRectangle(const Rectangle& rect, const Color32& color) {
	C2D_DrawRectSolid(rect.position.x, rect.position.y, 0, rect.size.x, rect.size.y, color.value);
}
Image Platform::NewTexture(Vector2Int size, bool pixelFiltering) {
	C3D_Tex* tex = new C3D_Tex();
	if (!C3D_TexInitVRAM(tex, size.x, size.y, GPU_TEXCOLOR::GPU_RGBA8))
		EXCEPTION("Failed to create texture with size %ix%i!", size.x, size.y);

	C3D_RenderTarget* rt = C3D_RenderTargetCreateFromTex(tex, GPU_TEXFACE::GPU_TEXFACE_2D, 0, -1);

	if (rt == nullptr) {
		EXCEPTION("Failed to create render target for texture with size %ix%i!", size.x, size.y);
	}

	C3D_RenderTargetClear(rt, C3D_ClearBits::C3D_CLEAR_COLOR, C2D_Color32(0, 0, 0, 255), 0);
	createdRenderTargets.push_back({ rt,tex });

	if (!pixelFiltering)
		C3D_TexSetFilter(tex, GPU_TEXTURE_FILTER_PARAM::GPU_LINEAR, GPU_TEXTURE_FILTER_PARAM::GPU_LINEAR);
	else
		C3D_TexSetFilter(tex, GPU_TEXTURE_FILTER_PARAM::GPU_NEAREST, GPU_TEXTURE_FILTER_PARAM::GPU_NEAREST);

	Tex3DS_SubTexture* st = new Tex3DS_SubTexture();
	st->width = size.x;
	st->height = size.y;
	st->left = 0;
	st->right = 1;
	st->top = 1;
	st->bottom = 0;

	return { tex };
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
