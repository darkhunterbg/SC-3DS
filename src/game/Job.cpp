#include "Job.h"
#include "Platform.h"
#include "Debug.h"
#include <cmath>

Semaphore JobSystem::semaphore = nullptr;
int JobSystem::threads = 0;

static uint32_t padding[15];

thread_local int CurrentThreadId;

static uint32_t padding2[15];

std::atomic_int JobSystem::workingThreads = 0;

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

	int nextJob = takenJobs.fetch_add(1, std::memory_order_seq_cst);

	while (nextJob < totalJobs) {
		Job job = jobs[nextJob];
		a(job.start, job.end);

		nextJob = takenJobs.fetch_add(1, std::memory_order_seq_cst);
	}

	workingThreads.fetch_sub(1, std::memory_order_seq_cst);

	//auto t = Platform::ElaspedTime();
	//while (remainingJobs > 0)
	//{
	//	t = Platform::ElaspedTime() - t;
	//	if( t> 3)
	//		EXCEPTION("JOBS still remain !");
	//} 

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

	workingThreads = std::min(threads, (int)jobs.size()) + 1;
	takenJobs = 0;

	if (workingThreads > 1) {
		Platform::ReleaseSemaphore(semaphore, workingThreads - 1);
	}

	ThreadWorkOnJob(0);

	auto time = Platform::ElaspedTime();
	while (workingThreads) {
		auto delta = Platform::ElaspedTime() - time;
		if (delta > 3)
			EXCEPTION("Main thread locked! Taken jobs %i/%i, working threads %i",
				(int)takenJobs, jobs.size(), (int)workingThreads);
	}
}

void JobSystem::Init()
{
	CurrentThreadId = 0;

	semaphore = Platform::CreateSemaphore();
	threads = Platform::StartThreads(ThreadWork);

}
