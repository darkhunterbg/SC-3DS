
#include "Platform.h"


#include <SDL.h>
#include <SDL_gpu.h>

#include <fstream>
#include <filesystem>

#include <GL/glew.h>

#include "Color.h"
#include "StringLib.h"
#include "SDL_FontCache.h"
#include "MathLib.h"

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
extern GPU_Image* white;

static ScreenId currentRT;
static GPU_Target* target = nullptr;
static Vertex* vertexBuffer = nullptr;


constexpr Uint8 SDL_FloatToUint8(float x) {
	return (Uint8)(255.0f * ClampF(x, 0.0f, 1.0f) + 0.5f);
}

static 	void AudioCallback(void* userdata, Uint8* stream, int len);


TextureId Platform::LoadTexture(const char* path, Vector2Int16& outSize) {

	std::filesystem::path p = assetDir;
	p.append(path);
	p.concat(".png");

	GPU_Image* tex = GPU_LoadImage(p.string().data());

	if (tex == nullptr) {
		auto error = SDL_GetError();
		EXCEPTION("Load texture %s failed with error %i.", path.data(), error);
	}

	outSize.x = (short)tex->w;
	outSize.y = (short)tex->h;

	return tex;

}
const Font* Platform::LoadFont(const char* path, int size) {

	std::filesystem::path fullPath = assetDir;
	fullPath = fullPath.parent_path().append("gfx").append(path).replace_extension("ttf");

	FC_Font* font = FC_CreateFont();
	auto lf = FC_LoadFont(font, fullPath.generic_string().data(), size, FC_MakeColor(255, 255, 255, 255), TTF_STYLE_NORMAL);


	if (lf == 0) {
		const char* error = SDL_GetError();
		EXCEPTION("Load font %s failed with %s!", fullPath.generic_string().data(), error);
		FC_ClearFont(font);
		return nullptr;
	}

	return new Font(font, 1);
}
Vector2Int Platform::MeasureString(const Font& font, const char* text) {
	FC_Font* f = font.GetFontId<FC_Font>();
	GPU_Rect rect = FC_GetBounds(f, 0, 0, FC_AlignEnum::FC_ALIGN_LEFT, FC_MakeScale(font.GetScale(), font.GetScale()), text);

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
void Platform::DrawOnSurface(SurfaceId surface) {
	auto t = (GPU_Target*)surface;

	if (t == nullptr)
		target = (screens[(int)currentRT])->target;
	else
		target = t;
}
SurfaceId Platform::NewRenderSurface(Vector2Int size, bool pixelFiltering, TextureId& outTexture) {
	GPU_Image* tex = nullptr;

	tex = GPU_CreateImage(size.x, size.y, GPU_FORMAT_RGBA);

	if (pixelFiltering) {
		GPU_SetImageFilter(tex, GPU_FILTER_NEAREST);
	}
	else
	{
		GPU_SetImageFilter(tex, GPU_FILTER_LINEAR);
	}

	GPU_Target* surface = GPU_LoadTarget(tex);
	GPU_SetWrapMode(tex, GPU_WRAP_NONE, GPU_WRAP_NONE);
	outTexture = tex;

	return surface;
}
SubImageCoord Platform::GenerateUV(TextureId texture, Rectangle16 src) {
	GPU_Image* img = (GPU_Image*)texture;
	Vector2 start = Vector2(src.position) / Vector2(img->w, img->h);
	Vector2 end = Vector2(src.GetMax()) / Vector2(img->w, img->h);

	SubImageCoord uv = SubImageCoord{ start,{end.x, start.y}, {start.x, end.y }, end };

	return uv;
}

void Platform::ChangeBlendingMode(BlendMode mode) {
	GPU_BlendPresetEnum b = GPU_BLEND_NORMAL;

	switch (mode)
	{
	case BlendMode::Alpha:
		b = GPU_BLEND_NORMAL;
		break;
	case BlendMode::AlphaSet:
		b = GPU_BLEND_SET_ALPHA;
		break;
	case BlendMode::AllSet:
		b = GPU_BLEND_SET;
		break;
	default:
		break;
	}

	GPU_SetShapeBlendMode(b);
}

void Platform::ClearBuffer(Color color) {
	Color32 c(color);

	GPU_ClearRGBA(target, c.GetR(), c.GetG(), c.GetB(), c.GetA());
}


Span<Vertex> Platform::GetVertexBuffer() {
	if (vertexBuffer == nullptr) {
		vertexBuffer = new Vertex[10 * 1024];
	}
	return { vertexBuffer, 10 * 1024 };
}
void Platform::ExecDrawCommands(const Span<DrawCommand> commands) {

	for (const DrawCommand& cmd : commands)
	{
		switch (cmd.type)
		{
		case DrawCommandType::TexturedTriangle: {

			GPU_Image* img = (GPU_Image*)cmd.texture->GetTextureId();

			GPU_PrimitiveBatchV(img, target, GPU_TRIANGLES, cmd.count, vertexBuffer + cmd.start, 0, nullptr, GPU_BATCH_XY_ST_RGBA8);

			break;
		}
		case DrawCommandType::Triangle: {
			for (int i = cmd.start; i < cmd.start + cmd.count; i += 6) {

				Vector2 start = vertexBuffer[i].position;
				Color32 c = vertexBuffer[i].color.value;
				SDL_Color color;

				color.r = c.GetR();
				color.g = c.GetG();
				color.b = c.GetB();
				color.a = c.GetA();

				Vector2 end = vertexBuffer[i + 2].position;

				GPU_RectangleFilled(target, start.x, start.y, end.x, end.y, color);
			}
			break;
		}
		default:
			break;
		}

	}
}

void Platform::DrawText(const Font& font, Vector2Int position, const char* text, Color color) {

	auto c = FC_MakeColor(SDL_FloatToUint8(color.r), SDL_FloatToUint8(color.g), SDL_FloatToUint8(color.b), SDL_FloatToUint8(color.a));


	FC_DrawScaleColor(font.GetFontId<FC_Font>(), target, position.x, position.y, FC_MakeScale(1, 1), c, text);

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
