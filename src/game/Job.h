#pragma once
#include "Span.h"
#include <atomic>
#include <vector>
#include <functional>


typedef void* Semaphore;



struct Job {
	int start;
	int end;
};

//static_assert(sizeof(Job) == 64, "Job is not 64 bytes!");

template <class T>
class ThreadLocal;

class JobSystem {
private:
	JobSystem() = delete;
	~JobSystem() = delete;

	static Semaphore semaphore;
	static int threads;
	static std::atomic_int workingThreads;
	static std::atomic_int takenJobs;
	static std::vector<Job> jobs;
	static std::function<void(int, int)> action;

	static void ThreadWorkOnJob(int threadId);
	static void ThreadWork(int threadId);
	static void ExecJob(int elements, int batchSize);

	template <class T>
	friend class ThreadLocal;
public:
	static void Init();
	static inline void RunJob(int elements, int batchSize,
		const std::function<void(int, int)>& a) {

		if (!elements)
			return;
		action = a;
		ExecJob(elements, batchSize);
	}

	static constexpr const int DefaultJobSize = 128;
	static constexpr const int SingleThreadJobSize = 0x7FFFFFFF;
};

extern thread_local int CurrentThreadId;


template <class T>
class ThreadLocal {
private:
	T* items;
	unsigned size;
public:
	ThreadLocal() {
		size = JobSystem::threads + 1;
		items = new T[size];
	}
	~ThreadLocal() {
		delete[] items;
	}

	inline T& Get() { return items[CurrentThreadId]; }
	inline Span<T> GetAll() { return { items, size }; }
};

