
#include "Platform.h"


#include <SDL.h>
#include <SDL_gpu.h>

#include <fstream>
#include <filesystem>

#include <GL/glew.h>

#include "SDLDrawCommand.h"
#include "Color.h"
#include "StringLib.h"
#include "SDL_FontCache.h"
#include "MathLib.h"
#include "Audio.h"
#include "Debug.h"

#include <stdio.h>

#include <SDL_ttf.h>


extern std::filesystem::path assetDir;
extern std::filesystem::path userDir;
extern GPU_Target* screen;
extern GPU_Image* screens[2];
extern uint64_t mainTimer;
extern Rectangle touchScreenLocation;
extern bool mute;
extern bool noThreading;
extern int numberOfThreads;

static ScreenId currentRT;
static GPU_Target* target = nullptr;

constexpr Uint8 SDL_FloatToUint8(float x) {
	return (Uint8)(255.0f * ClampF(x, 0.0f, 1.0f) + 0.5f);
}

static 	void AudioCallback(void* userdata, Uint8* stream, int len);

GPU_Image* LoadTexture(const std::string& path, Vector2Int& size) {
	GPU_Image* tex = GPU_LoadImage(path.data());

	if (tex == nullptr) {
		auto error = SDL_GetError();
		EXCEPTION("Load texture %s failed with error %i.", path.data(), error);
	}

	size = { tex->w, tex->h };

	return tex;
}

const SpriteAtlas* Platform::LoadAtlas(const char* path) {

	std::filesystem::path p = assetDir;
	p.append("gfx");
	p.append(path);
	p.replace_extension("t3s");

	std::ifstream file(p);

	if (!file.is_open())
	{
		EXCEPTION("Open atlas %s failed!", p.generic_string().data());
	}

	SpriteAtlas* asset = new SpriteAtlas();

	std::string str;
	std::getline(file, str);

	while (std::getline(file, str)) {
		std::filesystem::path f = assetDir;
		f.append("gfx");
		f.append(str);

		Vector2Int size;
		GPU_Image* tex = LoadTexture(f.generic_string(), size);
		if (tex == nullptr)
		{
			delete asset;
			return nullptr;
		}
		Rectangle16 rect = { {0,0},Vector2Int16(size) };
		Sprite s = { rect ,{ tex ,nullptr} };
		asset->AddSprite(s);
	}

	return asset;
}
Font Platform::LoadFont(const char* path) {

	std::filesystem::path fullPath = assetDir;
	fullPath.append("gfx").append(path).replace_extension("ttf");

	FC_Font* font = FC_CreateFont();
	auto lf = FC_LoadFont(font, fullPath.generic_string().data(), 30, FC_MakeColor(255, 255, 255, 255), TTF_STYLE_NORMAL);


	//if (lf == 0) {
	//	const char* error = SDL_GetError();
	//	EXCEPTION("Load font %s failed with %s!", fullPath.generic_string().data(), error);
	//	FC_ClearFont(font);
	//	return  { 0 };
	//}

	return { font };
}
Vector2Int Platform::MeasureString(Font font, const char* text, float scale) {
	FC_Font* f = font.GetFontId<FC_Font>();
	GPU_Rect rect = FC_GetBounds(f, 0, 0, FC_AlignEnum::FC_ALIGN_LEFT, FC_MakeScale(scale, scale), text);

	return { (int)rect.w, (int)rect.h };
}

FILE* Platform::OpenAsset(const char* path) {
	std::filesystem::path f = assetDir;
	f.append(path);

	return fopen(f.generic_string().data(), "rb");
}

void Platform::DrawOnScreen(ScreenId screen) {
	currentRT = screen;
	target = (screens[(int)currentRT])->target;

}
void Platform::DrawOnTexture(Texture texture) {
	auto t = (GPU_Image*)texture;

	if (t == nullptr)
		target = (screens[(int)currentRT])->target;
	else
		target = t->target;

}
Image Platform::NewTexture(Vector2Int size, bool pixelFiltering) {
	GPU_Image* tex = nullptr;

	tex = GPU_CreateImage(size.x, size.y, GPU_FORMAT_RGBA);

	if (pixelFiltering) {
		GPU_SetImageFilter(tex, GPU_FILTER_NEAREST);
	}
	else
	{
		GPU_SetImageFilter(tex, GPU_FILTER_LINEAR);
	}

	GPU_LoadTarget(tex);
	GPU_SetWrapMode(tex, GPU_WRAP_NONE, GPU_WRAP_NONE);
	return { tex, 0 };
}
Sprite Platform::NewSprite(Image image, Rectangle16 src) {
	return { src, image };
}


void Platform::ChangeBlendingMode(BlendMode mode) {
	GPU_BlendPresetEnum b = GPU_BLEND_NORMAL;

	switch (mode)
	{
	case BlendMode::Alpha:
		b = GPU_BLEND_NORMAL;
		break;
	case BlendMode::AlphaOverride:
		b = GPU_BLEND_SET_ALPHA;
		//blendMode = SDL_ComposeCustomBlendMode(
		//	SDL_BlendFactor::SDL_BLENDFACTOR_SRC_ALPHA, SDL_BlendFactor::SDL_BLENDFACTOR_ZERO,
		//	SDL_BlendOperation::SDL_BLENDOPERATION_ADD,
		//	SDL_BlendFactor::SDL_BLENDFACTOR_SRC_ALPHA, SDL_BlendFactor::SDL_BLENDFACTOR_ZERO,
		//	SDL_BlendOperation::SDL_BLENDOPERATION_ADD);
		break;
	case BlendMode::FullOverride:
		b = GPU_BLEND_SET;
		//blendMode = SDL_ComposeCustomBlendMode(
		//	SDL_BlendFactor::SDL_BLENDFACTOR_ONE, SDL_BlendFactor::SDL_BLENDFACTOR_ZERO,
		//	SDL_BlendOperation::SDL_BLENDOPERATION_ADD,
		//	SDL_BlendFactor::SDL_BLENDFACTOR_ONE, SDL_BlendFactor::SDL_BLENDFACTOR_ZERO,
		//	SDL_BlendOperation::SDL_BLENDOPERATION_ADD);
		break;
	default:
		break;
	}

	GPU_SetShapeBlendMode(b);
	//if (error) {
	//	const char* error = SDL_GetError();
	//	EXCEPTION(error);
	//}
}

void Platform::ClearBuffer(Color color) {
	Color32 c(color);

	GPU_ClearRGBA(target, c.GetR(), c.GetG(), c.GetB(), c.GetA());
	//SDL_RenderClear(renderer);
}
void Platform::BatchDraw(const Span<BatchDrawCommand> commands) {
	for (const auto& cmd : commands) {
		GPU_Image* img = (GPU_Image*)cmd.sprite.image.textureId;
		GPU_FlipEnum flags = cmd.scale.x < 0 ? GPU_FLIP_HORIZONTAL : GPU_FLIP_NONE;
		Vector2Int size;
	
		size.x = img->w;
		size.y = img->h;

		SDL_Rect dst = {
			(int)cmd.position.x,
			(int)cmd.position.y,
			(int)(size.x * std::abs(cmd.scale.x)),
			(int)(size.y * cmd.scale.y) };

		GPU_Rect src = { (float)0, (float)0, (float)size.x,(float)size.y };
		GPU_Rect dstC = { (float)dst.x,(float)dst.y, (float)dst.w,(float)dst.h };


		SDL_Color c;
		c.r = cmd.color.GetR();
		c.g = cmd.color.GetG();
		c.b = cmd.color.GetB();
		c.a = cmd.color.GetA();


		GPU_SetColor(img, c);

		GPU_BlitRectX(img, &src, target, &dstC,0,0,0, flags);
	}
}

static  Span<Vertex> b;

void Platform::SetBuffer(const Span< Vertex> buffer) {

	b = buffer;
	//GPU_FlushBlitBuffer();

	//GLPlatform::PrepareDraw();
	//GLPlatform::UpdateBuffer(buffer);

	//if (target != nullptr) {
	//	int h = GPU_GetTextureHandle(target->image);
	//	//GPU_TARGET_DATA* data = (GPU_TARGET_DATA*)target->data;
	//	glBindFramebuffer(GL_FRAMEBUFFER, h);
	//}
}
void Platform::DrawBuffer(unsigned start, unsigned count, Texture texture) {

	GPU_Image* img = (GPU_Image * )texture;

	GPU_TriangleBatchX(img, target, count, b.Data(), 0, nullptr, GPU_BATCH_XY_ST_RGBA8);

	//GLPlatform::DrawTriangles(start, count);
	//GPU_ResetRendererState();
}

void Platform::Draw(const Sprite& sprite, Rectangle dst, Color color, bool hFlip, bool vFlip) {
	SDLDrawCommand cmd;

	GPU_Image* img = (GPU_Image*)sprite.image.textureId;

	cmd.texture = (SDL_Texture*)sprite.image.textureId;
	cmd.src.x = sprite.rect.position.x;
	cmd.src.y = sprite.rect.position.y;
	cmd.src.w = sprite.rect.size.x;
	cmd.src.h = sprite.rect.size.y;
	cmd.dst = *(SDL_Rect*)&dst;

	GPU_Rect src = { (float)cmd.src.x, (float)cmd.src.y, (float)cmd.src.w,(float)cmd.src.h };
	GPU_Rect dstC = { (float)cmd.dst.x, (float)cmd.dst.y, (float)cmd.dst.w,(float)cmd.dst.h };

	cmd.r = SDL_FloatToUint8(color.r);
	cmd.g = SDL_FloatToUint8(color.g);
	cmd.b = SDL_FloatToUint8(color.b);
	cmd.a = SDL_FloatToUint8(color.a);


	SDL_Color c;
	c.r = cmd.r;
	c.g = cmd.g;
	c.b = cmd.b;
	c.a = cmd.a;

	cmd.type = SDLDrawCommandType::Sprite;

	cmd.flip = hFlip ? SDL_RendererFlip::SDL_FLIP_HORIZONTAL : SDL_RendererFlip::SDL_FLIP_NONE;
	if (vFlip) {
		cmd.flip = (SDL_RendererFlip)(cmd.flip | SDL_RendererFlip::SDL_FLIP_VERTICAL);
	}

	GPU_SetColor(img, c);

	GPU_BlitRect(img, &src, target, &dstC);
}
void Platform::DrawText(const Font& font, Vector2Int position, const char* text, Color color, float scale) {
	SDLDrawCommand cmd;

	cmd.type = SDLDrawCommandType::Text;
	cmd.font = font.GetFontId<FC_Font>();
	cmd.text = text;
	cmd.dst.x = position.x;
	cmd.dst.y = position.y;
	cmd.scale = scale;
	cmd.r = SDL_FloatToUint8(color.r);
	cmd.g = SDL_FloatToUint8(color.g);
	cmd.b = SDL_FloatToUint8(color.b);
	cmd.a = SDL_FloatToUint8(color.a);

	FC_DrawScaleColor(cmd.font, target, cmd.dst.x, cmd.dst.y, FC_MakeScale(cmd.scale, cmd.scale), FC_MakeColor(cmd.r, cmd.g, cmd.b, cmd.a), cmd.text.data());

}
void Platform::DrawLine(Vector2Int src, Vector2Int dst, Color color) {
	SDLDrawCommand cmd;

	cmd.type = SDLDrawCommandType::Line;
	cmd.r = SDL_FloatToUint8(color.r);
	cmd.g = SDL_FloatToUint8(color.g);
	cmd.b = SDL_FloatToUint8(color.b);
	cmd.a = SDL_FloatToUint8(color.a);
	cmd.src.x = src.x;
	cmd.src.y = src.y;
	cmd.dst.x = dst.x;
	cmd.dst.y = dst.y;


	SDL_Color c;
	c.r = cmd.r;
	c.g = cmd.g;
	c.b = cmd.b;
	c.a = cmd.a;

	GPU_Line(target, cmd.src.x, cmd.src.y, cmd.dst.x, cmd.dst.y, c);

}
void Platform::DrawRectangle(const Rectangle& rect, const Color32& color) {
	SDLDrawCommand cmd;

	cmd.type = SDLDrawCommandType::Rectangle;
	cmd.dst = *(SDL_Rect*)&rect;
	cmd.r = color.GetR();
	cmd.g = color.GetG();
	cmd.b = color.GetB();
	cmd.a = color.GetA();

	SDL_Color c;
	c.r = cmd.r;
	c.g = cmd.g;
	c.b = cmd.b;
	c.a = cmd.a;

	GPU_RectangleFilled(target, cmd.dst.x, cmd.dst.y, cmd.dst.x + cmd.dst.w, cmd.dst.y + cmd.dst.h, c);
}

double Platform::ElaspedTime() {
	uint64_t elapsed = SDL_GetPerformanceCounter() - mainTimer;

	double elapsedSeconds = (double)(elapsed) / (double)SDL_GetPerformanceFrequency();

	return elapsedSeconds;
}

void Platform::UpdateGamepadState(GamepadState& state) {
	static SDL_Joystick* joystick = nullptr;
	if (joystick == nullptr)
		joystick = SDL_JoystickOpen(0);
	if (!joystick)
		return;

	if (SDL_NumJoysticks() == 0) {
		SDL_JoystickClose(joystick);
		joystick = nullptr;
		return;
	}

	bool buttons[15];
	for (int i = 0; i < 15; ++i)
		buttons[i] = SDL_JoystickGetButton(joystick, i) > 0;

	static constexpr const Sint16 DEVICE_AXIS_DEADZONE = 8000;

	float axis[4];
	for (int i = 0; i < 4; ++i) {
		int value = SDL_JoystickGetAxis(joystick, i);
		int sign = value < 0 ? -1 : 1;
		value = abs(value);

		value = value > DEVICE_AXIS_DEADZONE ? value - DEVICE_AXIS_DEADZONE : 0;
		float val = (float)value / ((float)INT16_MAX - (float)DEVICE_AXIS_DEADZONE);
		val *= sign;
		axis[i] = val;
	}

	//int b = -1;
	//for (int i = 0; i < 4; ++i)
	//	if (axis[i] != 0) {
	//		b = i;
	//		__debugbreak();
	//	}

	//int b = -1;
	//for (int i = 0; i < 15; ++i)
	//	if (buttons[i] > 0) {
	//		b = i;
	//		__debugbreak();
	//	}

	state.Y = buttons[2];
	state.A = buttons[1];
	state.X = buttons[3];
	state.B = buttons[0];
	state.Select = buttons[4];
	state.Start = buttons[6];
	state.L = buttons[9];
	state.R = buttons[10];
	state.ZL = SDL_JoystickGetAxis(joystick, 4) > 0;//buttons[7];
	state.ZR = SDL_JoystickGetAxis(joystick, 5) > 0; //buttons[8];
	state.DPad = Vector2Int(buttons[14], buttons[12]) - Vector2Int(buttons[13], buttons[11]);
	state.CPad = Vector2(axis[0], axis[1]);
	state.CStick = Vector2(axis[2], axis[3]);

}
void Platform::UpdatePointerState(PointerState& state) {
	if (touchScreenLocation.size.x == 0 || touchScreenLocation.size.y == 0)
		return;

	Vector2Int pos;
	Uint32  buttonState = SDL_GetMouseState(&pos.x, &pos.y);
	state.Touch = buttonState & SDL_BUTTON(SDL_BUTTON_LEFT);


	pos -= Vector2Int(touchScreenLocation.position);
	pos.x = std::min(std::max(pos.x, 0), (int)touchScreenLocation.size.x);
	pos.y = std::min(std::max(pos.y, 0), (int)touchScreenLocation.size.x);
	pos.x = (pos.x * 320) / touchScreenLocation.size.x;
	pos.y = (pos.y * 240) / touchScreenLocation.size.y;

	state.Position = pos;
}

void Platform::CreateChannel(AudioChannelState& channel) {

	SDL_AudioSpec wavSpec = {};
	wavSpec.channels = channel.mono ? 1 : 2;
	wavSpec.format = AUDIO_S16;
	wavSpec.freq = 22050;
	wavSpec.samples = channel.bufferSize / 4;
	SDL_AudioSpec got;
	wavSpec.callback = AudioCallback;
	wavSpec.userdata = &channel;

	SDL_AudioDeviceID deviceId = SDL_OpenAudioDevice(nullptr, 0, &wavSpec, &got, 0);
	if (deviceId == 0) {
		channel.handle = deviceId;
		EXCEPTION("Failed to create audio channel!");
	}

	channel.bufferSize = got.samples * 4;
	channel.handle = deviceId;

	SDL_PauseAudioDevice(deviceId, 1);
}
void Platform::EnableChannel(const AudioChannelState& channel, bool enabled) {

	if (!mute)
		SDL_PauseAudioDevice(channel.handle, enabled ? 0 : 1);
}

static std::function<void(int)> threadWorkFunc;
int Platform::StartThreads(std::function<void(int)> threadWork) {
	if (noThreading)
		return 0;

	threadWorkFunc = threadWork;

	int numberOfThreads = SDL_GetCPUCount();

	for (int i = 1; i < numberOfThreads; ++i) {
		std::string name = "WorkerThread" + std::to_string(i);

		SDL_CreateThread([](void* data) {
			threadWorkFunc(*(int*)data);
			return 0;
			}, name.data(), new int(i));
	}
	return std::max(0, numberOfThreads - 1);
}
Semaphore Platform::CreateSemaphore() {
	return SDL_CreateSemaphore(0);
}
void Platform::WaitSemaphore(Semaphore s) {
	SDL_SemWait((SDL_sem*)s);
}
void Platform::ReleaseSemaphore(Semaphore s, int v) {
	for (int i = 0; i < v; ++i)
		SDL_SemPost((SDL_sem*)s);
}

std::string Platform::GetUserDirectory() {

	return userDir.u8string() + "/";
}

static void AudioCallback(void* userdata, Uint8* stream, int len) {
	AudioChannelState* state = (AudioChannelState*)userdata;

	SDL_memset(stream, 0, len);

	AudioChannelClip* clip = state->CurrentClip();

	unsigned size = clip != nullptr ? clip->Remaining() : 0;
	if (size == 0) {
		printf("Voice starvation at channel %i\n", state->handle);
		return;
	}

	len = (len > size ? size : len);

	int volume = (SDL_MIX_MAXVOLUME * state->volume) / 2;


	SDL_MixAudioFormat(stream, clip->PlayFrom(), AUDIO_S16LSB, len, volume);

	clip->playPos += len;

	if (clip->Done()) {
		state->DequeueClip();
	}
}
