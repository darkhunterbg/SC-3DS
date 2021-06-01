#include "Profiler.h"
#include "StringLib.h"
#include "Game.h"
#include "StringLib.h"

#include "Engine/GraphicsRenderer.h"

#include "Platform.h"

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
	GraphicsRenderer::DrawOnScreen(ScreenId::Top);

	char text[128];

	Color c = Colors::White;
	if (frameTime > 16.7)
		c = Colors::Orange;
	if (frameTime > 33.4)
		c = Colors::Red;

	stbsp_snprintf(text, sizeof(text), "Frame %0.1f ms", frameTime);
	GraphicsRenderer::DrawText(*Game::SystemFont12, { 1,0 }, text, c);

	int y = 15;
	for (const auto& profile : profileData) {
		stbsp_snprintf(text, sizeof(text), "%s %0.1f ms", profile.first.data(), profile.second);
		GraphicsRenderer::DrawText(*Game::SystemFont12, { 1,y }, text, Colors::White);
		y += 15;
	}

	stbsp_snprintf(text, sizeof(text), "Time %i s", (int)Platform::ElaspedTime());
	GraphicsRenderer::DrawText(*Game::SystemFont12, { 1,y }, text, Colors::White);
	y += 15;


	if (frameLoad.size() == 0)
		return;

	Vector2Int offset = { 60 ,y + 50 };


	float max = *std::max_element(frameLoad.begin(), frameLoad.end());

	scale = std::fmax(1, max);

	for (int i = 0; i < frameLoad.size(); ++i) {
		int y = std::fmax(1, ((frameLoad[i] * 50) / scale));
		c = Colors::LightGreen;
		if (frameLoad[i] > 1)
			c = Colors::Orange;
		if (frameLoad[i] > 2)
			c = Colors::Red;
		GraphicsRenderer::DrawLine(offset, offset + Vector2Int(0, -y), c);
		offset.x--;
	}
	offset = { 1, y };

	GraphicsRenderer::DrawLine(offset, offset + Vector2Int(60, 0), Colors::White);
	offset.y += 50;
	GraphicsRenderer::DrawLine(offset, offset + Vector2Int(60, 0), Colors::White);
}

void Profiler::FrameStart() {
	frameStartTime = Platform::ElaspedTime();;

}
void Profiler::FrameEnd() {
	auto now = Platform::ElaspedTime();
	double thisFrameTime = (now - frameStartTime) * 1000;

	if (thisFrameTime > 1000)
		return;

	frameTime =  std::roundf((frameTime * 0.9 + thisFrameTime * 0.1) * 10.0) / 10.0;
	frameLoad.insert(frameLoad.begin(), thisFrameTime / 16.7);

	if (frameLoad.size() >= 60)
		frameLoad.erase(frameLoad.end() - 1);

}
void Profiler::AddStat(const char* name, double timeMs) {
	double r = profileData[name];
	if (timeMs > 1000)
		return;
	profileData[name] = std::roundf((r * 0.9 + timeMs * 0.1) * 10.0) / 10.0;
}

void Profiler::AddCounter(const char* name, int  value) {
	int r = profileData[name];

	profileData[name] = (int)(  value);
}