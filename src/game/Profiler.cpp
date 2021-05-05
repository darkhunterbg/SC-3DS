#include "Profiler.h"
#include "Platform.h"
#include "StringLib.h"
#include "Game.h"
#include "StringLib.h"

#include <algorithm>
#include <vector>
#include <unordered_map>


SectionProfiler::SectionProfiler(const char* text, ...) {
	va_list args;
	va_start(args, text);
	stbsp_vsnprintf(name, sizeof(name), text, args);
	va_end(args);

	start = Platform::ElaspedTime();
}

SectionProfiler::~SectionProfiler() {
	if (recorded)
		return;
	auto time = (Platform::ElaspedTime() - start) * 1000;
	Profiler::AddStat(name, time);
}
void SectionProfiler::Submit() {
	auto time = (Platform::ElaspedTime() - start) * 1000;
	Profiler::AddStat(name, time);
	recorded = true;
}

static std::vector<double> frameLoad;
static double frameStartTime = 0;
static double frameTime = 0;
static std::unordered_map<std::string, double> profileData;
static float scale = 1.0f;

void Profiler::ShowPerformance() {
	Platform::DrawOnScreen(ScreenId::Top);

	char text[128];

	Color c = Colors::White;
	if (frameTime > 16.67)
		c = Colors::Orange;
	if (frameTime > 33.4)
		c = Colors::Red;

	stbsp_snprintf(text, sizeof(text), "Frame %0.1f ms", frameTime);
	Platform::DrawText(Game::SystemFont, { 1,0 }, text, c, 0.4f);

	int y = 15;
	for (const auto& profile : profileData) {
		stbsp_snprintf(text, sizeof(text), "%s %0.1f ms", profile.first.data(), profile.second);
		Platform::DrawText(Game::SystemFont, { 1,y }, text, Colors::White, 0.4f);
		y += 15;
	}

	stbsp_snprintf(text, sizeof(text), "Time %i s", (int)Platform::ElaspedTime());
	Platform::DrawText(Game::SystemFont, { 1,y }, text, Colors::White, 0.4f);
	y += 15;


	if (frameLoad.size() == 0)
		return;

	Vector2Int offset = { 61 ,y + 50 };


	float max = *std::max_element(frameLoad.begin(), frameLoad.end());

	if (max < scale)
		scale = std::fmax(1, scale / 2.0f);
	else scale = max;

	for (int i = 0; i < frameLoad.size(); ++i) {
		int y = std::fmax(1, ((frameLoad[i] * 50) / (int)scale));
		c = Colors::LightGreen;
		if (frameLoad[i] > 1)
			c = Colors::Orange;
		if (frameLoad[i] > 2)
			c = Colors::Red;
		Platform::DrawLine(offset, offset + Vector2Int(0, -y), c);
		offset.x--;
	}
	offset = { 1, y };

	Platform::DrawLine(offset, offset + Vector2Int(60, 0), Colors::White);
	offset.y += 50;
	Platform::DrawLine(offset, offset + Vector2Int(60, 0), Colors::White);
}

void Profiler::FrameStart() {
	frameStartTime = Platform::ElaspedTime();;

}
void Profiler::FrameEnd() {
	auto now = Platform::ElaspedTime();
	double thisFrameTime = (now - frameStartTime) * 1000;
	frameTime = std::roundf((frameTime * 0.9 + thisFrameTime * 0.1) * 10.0) / 10.0;
	frameLoad.insert(frameLoad.begin(), thisFrameTime / 16.67);
	if (frameLoad.size() >= 60)
		frameLoad.erase(frameLoad.end() - 1);

}
void Profiler::AddStat(const char* name, double timeMs) {
	double r = profileData[name];
	profileData[name] = std::roundf((r * 0.9 + timeMs * 0.1) * 10.0) / 10.0;
}