
#include "Platform.h"
#include "AbstractPlatform.h"
#include "lodepng.h"

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
extern AbstractPlatform abstractPlatform;

extern uint64_t mainTimer;
extern bool mute;
extern bool noThreading;
extern int numberOfThreads;
extern GPU_Image* white;

static ScreenId currentRT;
static GPU_Target* target = nullptr;

GPU_BlendPresetEnum blendMode;

constexpr Uint8 SDL_FloatToUint8(float x)
{
	return (Uint8)(255.0f * ClampF(x, 0.0f, 1.0f) + 0.5f);
}

static 	void AudioCallback(void* userdata, Uint8* stream, int len);

PlatformInfo Platform::GetPlatformInfo()
{
	PlatformInfo info;
	for (auto& s : abstractPlatform.Screens)
		info.Screens.push_back(s.Resolution);

	info.PointerIsCursor = abstractPlatform.Pointer.SameAsCursor;
	info.Type = abstractPlatform.Type;
	info.HardwareThreadsCount = 1;

	if (!noThreading)
		info.HardwareThreadsCount = SDL_GetCPUCount();

	return info;
}

TextureId Platform::CreateTextureFromFile(const char* path, Span<uint8_t> data, Vector2Int16& outSize)
{
	uint8_t* decoded = nullptr;
	unsigned w, h;
	int error = lodepng_decode32(&decoded, &w, &h, data.Data(), data.Size());
	if (error != 0) EXCEPTION("Decode texture data %s failed with error %i.", path.data(), error);

	outSize.x = w;
	outSize.y = h;

	GPU_Image* tex = (GPU_Image*)NewTexture(Vector2Int(outSize), false);

	
	GPU_Rect rect = { 0.0f,0.0f, (float)w, (float)h };

	UpdateTexture(tex, { {0,0},{(int)w,(int)h} }, { decoded, w * h * 4 });

	delete[] decoded;

	return tex;
}

const Font* Platform::LoadFont(const char* path, int size)
{
	std::filesystem::path fullPath = assetDir;
	fullPath = fullPath.append(path).replace_extension("ttf");

	FC_Font* font = FC_CreateFont();
	auto lf = FC_LoadFont(font, fullPath.generic_string().data(), size, FC_MakeColor(255, 255, 255, 255), TTF_STYLE_NORMAL);

	if (lf == 0)
	{
		const char* error = SDL_GetError();
		EXCEPTION("Load font %s failed with %s!", fullPath.generic_string().data(), error);
		FC_ClearFont(font);
		return nullptr;
	}

	return new Font(font, 1);
}
Vector2Int Platform::MeasureString(const Font& font, const char* text)
{
	FC_Font* f = font.GetFontId<FC_Font>();
	GPU_Rect rect = FC_GetBounds(f, 0, 0, FC_AlignEnum::FC_ALIGN_LEFT, FC_MakeScale(font.GetScale(), font.GetScale()), text);

	return { (int)rect.w, (int)rect.h };
}

FILE* Platform::OpenAsset(const char* path, AssetType type)
{
	std::filesystem::path f = assetDir;
	f.append(path);

	switch (type)
	{
	case AssetType::Unknown:
		break;
	case AssetType::Texture:
		f.replace_extension("png"); break;
	case AssetType::Font:
		f.replace_extension("ttf"); break;
	case AssetType::AudioClip:
		f.replace_extension("wav"); break;
	case AssetType::VideoClip:
		f.replace_extension("smk"); break;
	case AssetType::Database:
		f.replace_extension("bin"); break;
		break;
	default:
		break;
	}

	return fopen(f.generic_string().data(), "rb");
}

void Platform::DrawOnScreen(ScreenId screen)
{
	int id = (int)screen;

	if (id >= abstractPlatform.Screens.size())
		return;

	currentRT = screen;
	target = abstractPlatform.Screens[id].Image->target;

}
void Platform::DrawOnSurface(SurfaceId surface)
{
	auto t = (GPU_Target*)surface;

	if (t == nullptr)
		target = (abstractPlatform.Screens[(int)currentRT]).Image->target;
	else
		target = t;
}
SurfaceId Platform::NewRenderSurface(Vector2Int size, bool pixelFiltering, TextureId& outTexture)
{
	GPU_Image* tex = nullptr;

	tex = (GPU_Image*)NewTexture(size, pixelFiltering);


	GPU_Target* surface = GPU_LoadTarget(tex);
	GPU_SetWrapMode(tex, GPU_WRAP_NONE, GPU_WRAP_NONE);
	outTexture = tex;

	return surface;
}
TextureId Platform::NewTexture(Vector2Int size, bool pixelFiltering)
{
	GPU_Image* tex = GPU_CreateImage(size.x, size.y, GPU_FORMAT_RGBA);

	if (tex == nullptr)
	{
		auto error = SDL_GetError();
		EXCEPTION("Load texture %s failed with error %i.", path.data(), error);
	}

	if (pixelFiltering)
	{
		GPU_SetImageFilter(tex, GPU_FILTER_NEAREST);
	}
	else
	{
		GPU_SetImageFilter(tex, GPU_FILTER_LINEAR);
	}

	return tex;
}
void Platform::DestroyTexture(TextureId texture)
{
	GPU_FreeImage((GPU_Image*)texture);
}
void Platform::UpdateTexture(TextureId texture, Rectangle part, Span<uint8_t> bytes)
{
	auto t = (GPU_Image*)texture;

	GPU_Rect rect = { (float)part.position.x, (float)part.position.y, (float)part.size.x, (float)part.size.y };

	GPU_UpdateImageBytes(t, &rect, bytes.Data(), part.size.x * 4);
}
SubImageCoord Platform::GenerateUV(TextureId texture, Rectangle16 src)
{
	GPU_Image* img = (GPU_Image*)texture;
	Vector2 start = Vector2(src.position) / Vector2(img->w, img->h);
	Vector2 end = Vector2(src.GetMax()) / Vector2(img->w, img->h);

	SubImageCoord uv = SubImageCoord{ start,{end.x, start.y}, {start.x, end.y }, end };

	return uv;
}
void Platform::ChangeBlendingMode(BlendMode mode)
{
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

	blendMode = b;

	GPU_SetShapeBlendMode(b);
}
void Platform::ClearBuffer(Color color)
{
	Color32 c(color);

	GPU_ClearRGBA(target, c.GetR(), c.GetG(), c.GetB(), c.GetA());
}
void Platform::DrawTexture(const Texture& texture, const SubImageCoord& uv, const Rectangle16& dst, bool hFlip, Color32 c)
{
	GPU_Image* img = (GPU_Image*)texture.GetTextureId();
	Rectangle16 src = uv.GetSource(texture.GetSize());
	GPU_Rect srcRect = { (float)src.position.x, (float)src.position.y, (float)src.size.x, (float)src.size.y };
	Vector2 scale = Vector2(dst.size) / Vector2(src.size);
	scale.x -= scale.x * 2.0f * hFlip;

	GPU_SetRGBA(img, c.GetR(), c.GetG(), c.GetB(), c.GetA());
	GPU_SetBlendMode(img, blendMode);
	GPU_BlitScale(img, &srcRect, target, (float)dst.GetCenter().x, (float)dst.GetCenter().y, scale.x, scale.y);
}
void Platform::DrawRectangle(const Rectangle& rect, Color32 c)
{
	SDL_Color sdlColor;

	sdlColor.r = c.GetR();
	sdlColor.g = c.GetG();
	sdlColor.b = c.GetB();
	sdlColor.a = c.GetA();

	GPU_SetRGBA(white, c.GetR(), c.GetG(), c.GetB(), c.GetA());

	GPU_RectangleFilled(target, rect.position.x, rect.position.y, rect.GetMax().x, rect.GetMax().y, sdlColor);
}
void Platform::DrawText(const Font& font, Vector2Int position, const char* text, Color color)
{
	auto c = FC_MakeColor(SDL_FloatToUint8(color.r), SDL_FloatToUint8(color.g), SDL_FloatToUint8(color.b), SDL_FloatToUint8(color.a));

	FC_DrawScaleColor(font.GetFontId<FC_Font>(), target, position.x, position.y, FC_MakeScale(1, 1), c, text);
}

double Platform::ElaspedTime()
{
	uint64_t elapsed = SDL_GetPerformanceCounter() - mainTimer;

	double elapsedSeconds = (double)(elapsed) / (double)SDL_GetPerformanceFrequency();

	return elapsedSeconds;
}

void Platform::UpdateGamepadState(GamepadState& state)
{
	static SDL_Joystick* joystick = nullptr;
	if (joystick == nullptr)
		joystick = SDL_JoystickOpen(0);
	if (!joystick)
		return;

	if (SDL_NumJoysticks() == 0)
	{
		SDL_JoystickClose(joystick);
		joystick = nullptr;
		return;
	}

	bool buttons[15];
	for (int i = 0; i < 15; ++i)
		buttons[i] = SDL_JoystickGetButton(joystick, i) > 0;

	static constexpr const Sint16 DEVICE_AXIS_DEADZONE = 8000;

	float axis[4];
	for (int i = 0; i < 4; ++i)
	{
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
void Platform::UpdatePointerState(PointerState& state)
{
	state.Position = abstractPlatform.Pointer.Position;

	if (!abstractPlatform.Pointer.InsideScreen) return;

	Vector2Int pos;
	Uint32  buttonState = SDL_GetMouseState(&pos.x, &pos.y);

	state.Touch = buttonState & SDL_BUTTON(SDL_BUTTON_LEFT);
}

void Platform::CreateChannel(AudioChannelState& channel)
{

	SDL_AudioSpec wavSpec = {};
	wavSpec.channels = channel.mono ? 1 : 2;
	wavSpec.format = AUDIO_S16;
	wavSpec.freq = 22050;
	wavSpec.samples = channel.bufferSize / 4;
	SDL_AudioSpec got;
	wavSpec.callback = AudioCallback;
	wavSpec.userdata = &channel;

	SDL_AudioDeviceID deviceId = SDL_OpenAudioDevice(nullptr, 0, &wavSpec, &got, 0);
	if (deviceId == 0)
	{
		channel.handle = deviceId;
		EXCEPTION("Failed to create audio channel!");
	}

	channel.bufferSize = got.samples * 4;
	channel.handle = deviceId;

	SDL_PauseAudioDevice(deviceId, 1);
}
void Platform::EnableChannel(const AudioChannelState& channel, bool enabled)
{
	if (!mute)
		SDL_PauseAudioDevice(channel.handle, enabled ? 0 : 1);
}

static std::vector< std::function<void(int)>> threadWorkFunc;

int Platform::TryStartThreads(ThreadUsageType type, int requestCount, std::function<void(int)> threadWork)
{
	if (noThreading) return 0;

	for (int i = 0; i < requestCount; ++i)
	{
		threadWorkFunc.push_back(threadWork);
		int id = threadWorkFunc.size();
		std::string name;

		switch (type)
		{
		case ThreadUsageType::IO: name = "IOThread"; break;
		case ThreadUsageType::JobSystem: name = "WorkerThread"; break;
		default: name = "GenericThread"; break;
		}

		name += "_" + std::to_string(i);

		SDL_CreateThread([](void* data) {
			int i = *(int*)data;
			threadWorkFunc[i - 1](i);
			return 0;
			}, name.data(), new int(id));
	}
	return requestCount;
}
Semaphore Platform::CreateSemaphore()
{
	return SDL_CreateSemaphore(0);
}
void Platform::WaitSemaphore(Semaphore s)
{
	SDL_SemWait((SDL_sem*)s);
}
void Platform::ReleaseSemaphore(Semaphore s, int v)
{
	for (int i = 0; i < v; ++i)
		SDL_SemPost((SDL_sem*)s);
}

std::string Platform::GetUserDirectory()
{

	return userDir.u8string() + "/";
}

static void AudioCallback(void* userdata, Uint8* stream, int len)
{
	AudioChannelState* state = (AudioChannelState*)userdata;

	SDL_memset(stream, 0, len);

	AudioChannelClip* clip = state->CurrentClip();

	unsigned size = clip != nullptr ? clip->Remaining() : 0;
	if (size == 0)
	{
		printf("Voice starvation at channel %i\n", state->handle);
		return;
	}

	len = (len > size ? size : len);

	int volume = (SDL_MIX_MAXVOLUME * state->volume) / 2;


	SDL_MixAudioFormat(stream, clip->PlayFrom(), AUDIO_S16LSB, len, volume);

	clip->playPos += len;

	if (clip->Done())
	{
		state->DequeueClip();
	}
}
