#include "Job.h"
#include "Platform.h"
#include <cmath>

Semaphore JobSystem::semaphore = nullptr;
int JobSystem::threads = 0;
std::atomic_int JobSystem::remainingJobs = 0;
std::atomic_int JobSystem::takenJobs = 0;
std::vector<Job> JobSystem::jobs;
std::function<void(int, int)> JobSystem::action = 0;
thread_local int CurrentThreadId;

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
	if (!threads) {
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
