#pragma once

class SectionProfiler {
public:
	SectionProfiler(const char* name, ...);
	void Submit();
	~SectionProfiler();
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
};