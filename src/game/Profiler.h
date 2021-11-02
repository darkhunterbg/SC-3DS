#pragma once

class SectionProfiler {
public:
	SectionProfiler(const char* name, ...);
	void Submit();
	~SectionProfiler();
	double statMinThreshold = 0.0f;
private:
	char name[32];
	double start;
	bool recorded = false;
};

 class Profiler {
private:
	Profiler() = delete;
	~Profiler() = delete;
 public:
	 static void ShowPerformance();
	 static void FrameStart();
	 static void FrameEnd();
	 static void AddStat(const char* name, double timeMs);
	 static void AddCounter(const char* name, int value);
	 static void ClearStat(const char* name);
};