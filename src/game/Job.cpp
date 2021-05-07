#include "Job.h"
#include "Platform.h"
#include <cmath>

Semaphore JobSystem::semaphore = nullptr;
int JobSystem::threads = 0;

static uint32_t padding[15];

thread_local int CurrentThreadId;

static uint32_t padding2[15];

std::atomic_int JobSystem::remainingJobs = 0;

static uint32_t padding3[15];

std::atomic_int JobSystem::takenJobs = 0;

static uint32_t padding4[15];

std::vector<Job> JobSystem::jobs;
std::function<void(int, int)> JobSystem::action = 0;


void JobSystem::ThreadWork(int threadId)
{
	CurrentThreadId = threadId;

	while (true) {
		Platform::WaitSemaphore(semaphore);
		ThreadWorkOnJob(threadId);
	}
}

void JobSystem::ThreadWorkOnJob(int threadId)
{
	int totalJobs = jobs.size();
	auto a = action;

	int nextJob = takenJobs++;

	while (nextJob < totalJobs) {
		Job job = jobs[nextJob];
		a(job.start, job.end);

		--remainingJobs;

		nextJob = takenJobs.fetch_add(1);
	}
}

void JobSystem::ExecJob(int elements, int batchSize)
{
	if (!threads || elements <= batchSize) {
		action(0, elements);
		return;
	}


	jobs.clear();

	for (int i = 0; i < elements; i += batchSize) {
		int count = std::min(batchSize, elements - i);
		jobs.push_back({ i, i + count });
	}

	remainingJobs = jobs.size();
	takenJobs = 0;

	int awake = std::min(threads, (int)jobs.size() - 1);
	if (awake > 0) {
		Platform::ReleaseSemaphore(semaphore, awake);
	}

	ThreadWorkOnJob(0);
	while (remainingJobs);
}

void JobSystem::Init()
{
	CurrentThreadId = 0;

	semaphore = Platform::CreateSemaphore();
	threads = Platform::StartThreads(ThreadWork);

}
