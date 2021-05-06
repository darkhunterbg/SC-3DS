#pragma once
#include "Span.h"
#include <atomic>
#include <vector>


typedef void* Semaphore;

struct Job {
	int start;
	int end;
	int padding[14];
};

static_assert(sizeof(Job) == 64, "Job is not 64 bytes!");

class JobSystem {
private:
	JobSystem() = delete;
	~JobSystem() =delete;

	static Semaphore semaphore;
	static int threads;
	static std::atomic_int remainingJobs;
	static std::atomic_int takenJobs;
	static std::vector<Job> jobs;
	static void(*action)( int, int);

	static void ThreadWorkOnJob();
	static void ThreadWork(int threadId);
	static void ExecJob( int elements, int batchSize);
public:
	static void Init();
	static inline void RunJob(int elements, int batchSize,
		void(*a)( int, int)) {
		action = a;
		ExecJob(elements, batchSize);
	}
};