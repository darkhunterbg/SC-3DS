#include "Platform.h"

#include <SDL.h>
#include <fstream>
#include <filesystem>

#include "lodepng.h"
#include "SDLDrawCommand.h"
#include "Color.h"
#include "StringLib.h"
#include "SDL_FontCache.h"
#include "MathLib.h"
#include "Audio.h"
#include <stdio.h>

#include <SDL_ttf.h>

extern SDL_Renderer* renderer;
extern SDL_Window* window;
extern std::filesystem::path assetDir;
extern SDL_Texture* screens[2];
extern uint64_t mainTimer;
extern Rectangle touchScreenLocation;
extern bool mute;
extern int numberOfThreads;

static ScreenId currentScreen;
static SDL_Texture* target = nullptr;

constexpr Uint8 SDL_FloatToUint8(float x) {
	return (Uint8)(255.0f * ClampF(x, 0.0f, 1.0f) + 0.5f);
}

static 	void AudioCallback(void* userdata, Uint8* stream, int len);

SDL_Texture* LoadTexture(const std::string& path, Vector2Int& size) {
	std::vector<unsigned char> buffer, image;
	lodepng::load_file(buffer, path);
	unsigned width = 0, height = 0;
	unsigned error = lodepng::decode(image, width, height, buffer);
	if (error) {
		EXCEPTION("Load texture %s failed with error %i.", path.data(), error);
	}

	SDL_Texture* tex = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGBA32, SDL_TEXTUREACCESS_STATIC,
		width, height);

	SDL_UpdateTexture(tex, nullptr, image.data(), width * 4);
	size = { (int)width,(int)height };

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
		EXCEPTION("Open atlas %s failed!", path);
	}

	SpriteAtlas* asset = new SpriteAtlas();

	std::string str;
	std::getline(file, str);

	while (std::getline(file, str)) {
		std::filesystem::path f = assetDir;
		f.append("gfx");
		f.append(str);

		Vector2Int size;
		SDL_Texture* tex = LoadTexture(f.generic_string(), size);
		if (tex == nullptr)
		{
			delete asset;
			return nullptr;
		}
		Rectangle rect = { {0,0},size };
		Sprite s = { rect ,{ tex ,nullptr} };
		asset->AddSprite(s);
	}

	return asset;
}
Font Platform::LoadFont(const char* path) {

	std::filesystem::path fullPath = assetDir;
	fullPath.append("gfx").append(path).replace_extension("ttf");

	FC_Font* font = FC_CreateFont();
	auto lf = FC_LoadFont(font, renderer, fullPath.generic_string().data(), 30, FC_MakeColor(255, 255, 255, 255), TTF_STYLE_NORMAL);


	if (lf == 0) {
		const char* error = SDL_GetError();
		EXCEPTION("Load font %s failed with %s!", path, error);
		FC_ClearFont(font);
		return  { 0 };
	}

	return { font };
}

FILE* Platform::OpenAsset(const char* path) {
	std::filesystem::path f = assetDir;
	f.append(path);

	return fopen(f.generic_string().data(), "rb");
}

void Platform::DrawOnScreen(ScreenId screen) {
	currentScreen = screen;
	target = nullptr;
	SDL_SetRenderTarget(renderer, screens[(int)currentScreen]);
}
void Platform::DrawOnTexture(Texture texture) {
	target = (SDL_Texture*)texture;
	if (target == nullptr)
		DrawOnScreen(currentScreen);
	else
		SDL_SetRenderTarget(renderer, target);

}
Image Platform::NewTexture(Vector2Int size) {
	SDL_Texture* tex = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_TARGET, size.x, size.y);
	return { tex, 0 };
}


void Platform::BatchDraw(Span<BatchDrawCommand> commands) {
	for (const auto& cmd : commands) {
		auto texture = (SDL_Texture*)cmd.image.textureId;
		SDL_RendererFlip flags = cmd.scale.x < 0 ? SDL_RendererFlip::SDL_FLIP_HORIZONTAL : SDL_RendererFlip::SDL_FLIP_NONE;
		Vector2Int size;
		Uint32 f;
		int access;
		SDL_QueryTexture(texture, &f, &access, &size.x, &size.y);
		SDL_Rect dst = {
			(int)cmd.position.x,
			(int)cmd.position.y,
			(int)(size.x * std::abs(cmd.scale.x)),
			(int)(size.y * cmd.scale.y) };

		Uint8 r = cmd.color.color.GetR();
		Uint8 g = cmd.color.color.GetG();
		Uint8 b = cmd.color.color.GetB();
		Uint8 a = cmd.color.color.GetA();

		SDL_SetTextureBlendMode(texture, SDL_BlendMode::SDL_BLENDMODE_BLEND);
		SDL_SetTextureAlphaMod(texture, a);
		if (r != 0 || g != 0 || b != 0 || a != 255) {
			SDL_SetTextureColorMod(texture, r, g, b);
		}
		else {
			SDL_SetTextureColorMod(texture, 255, 255, 255);
		}

		SDL_RenderCopyEx(renderer, texture, nullptr, &dst, 0, nullptr, flags);
	}
}
void Platform::Draw(const Sprite& sprite, Rectangle dst, Color color, bool hFlip) {
	SDLDrawCommand cmd;

	cmd.texture = (SDL_Texture*)sprite.image.textureId;
	cmd.src = *(SDL_Rect*)&sprite.rect;
	cmd.dst = *(SDL_Rect*)&dst;

	cmd.r = SDL_FloatToUint8(color.r);
	cmd.g = SDL_FloatToUint8(color.g);
	cmd.b = SDL_FloatToUint8(color.b);
	cmd.a = SDL_FloatToUint8(color.a);
	cmd.type = SDLDrawCommandType::Sprite;
	cmd.flip = hFlip ? SDL_RendererFlip::SDL_FLIP_HORIZONTAL : SDL_RendererFlip::SDL_FLIP_NONE;

	SDL_SetTextureAlphaMod(cmd.texture, cmd.a);
	SDL_SetTextureBlendMode(cmd.texture, SDL_BlendMode::SDL_BLENDMODE_BLEND);

	if (cmd.r != 0 || cmd.g != 0 || cmd.b != 0 || cmd.a != 255) {
		SDL_SetTextureColorMod(cmd.texture, cmd.r, cmd.g, cmd.b);
	}
	else {
		SDL_SetTextureColorMod(cmd.texture, 255, 255, 255);
	}

	SDL_RenderCopyEx(renderer, cmd.texture, &cmd.src, &cmd.dst, 0, nullptr, cmd.flip);

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

	FC_DrawScaleColor(cmd.font, renderer, cmd.dst.x, cmd.dst.y, FC_MakeScale(cmd.scale, cmd.scale), FC_MakeColor(cmd.r, cmd.g, cmd.b, cmd.a), cmd.text.data());

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


	SDL_SetRenderDrawColor(renderer, cmd.r, cmd.g, cmd.b, cmd.a);
	SDL_RenderDrawLine(renderer, cmd.src.x, cmd.src.y, cmd.dst.x, cmd.dst.y);

}
void Platform::DrawRectangle(Rectangle rect, Color color) {
	SDLDrawCommand cmd;

	cmd.type = SDLDrawCommandType::Rectangle;
	cmd.dst = *(SDL_Rect*)&rect;
	cmd.r = SDL_FloatToUint8(color.r);
	cmd.g = SDL_FloatToUint8(color.g);
	cmd.b = SDL_FloatToUint8(color.b);
	cmd.a = SDL_FloatToUint8(color.a);

	SDL_SetRenderDrawColor(renderer, cmd.r, cmd.g, cmd.b, cmd.a);
	SDL_RenderFillRect(renderer, &cmd.dst);
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

	pos -= touchScreenLocation.position;
	pos.x = std::min(std::max(pos.x, 0), touchScreenLocation.size.x);
	pos.y = std::min(std::max(pos.y, 0), touchScreenLocation.size.x);
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
	threadWorkFunc = threadWork;

	numberOfThreads =  SDL_GetCPUCount();

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


static 	void AudioCallback(void* userdata, Uint8* stream, int len) {
	AudioChannelState* state = (AudioChannelState*)userdata;

	SDL_memset(stream, 0, len);

	AudioChannelClip* clip = state->CurrentClip();

	unsigned size = clip != nullptr ? clip->Remaining() : 0;
	if (size == 0) {
		printf("Voice starvation at channel %i\n", state->handle);
		return;
	}

	len = (len > size ? size : len);

	SDL_MixAudioFormat(stream, clip->PlayFrom(), AUDIO_S16LSB, len, SDL_MIX_MAXVOLUME / 2);

	clip->playPos += len;

	if (clip->Done()) {
		state->DequeueClip();
	}
}
