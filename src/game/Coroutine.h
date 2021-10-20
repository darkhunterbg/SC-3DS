#pragma once

#include <functional>
#include <vector>
#include <memory>

#define NAMEOF(CLASS) #CLASS

#define CRT_START()  virtual bool MoveNext() override { switch(__iter) { case 0:  

#define CRT_END() return true; }}

#define CRT_YIELD()  __iter = __LINE__ ; return  false;  case __LINE__: {}

#define CRT_WAIT_FOR(CRT) __waitFor = CRT; CRT_YIELD()

#define CRT_BREAK() return true; 

class CoroutineImpl;

typedef std::shared_ptr<CoroutineImpl> Coroutine;

class CoroutineImpl {
protected:
	int __iter = 0;

	Coroutine __waitFor = nullptr;

	virtual bool MoveNext() = 0;

public:

	CoroutineImpl() {}
	CoroutineImpl(const CoroutineImpl&) = delete;
	CoroutineImpl& operator=(const CoroutineImpl&) = delete;

	// Returns TRUE when coroutine is done.
	bool Next()
	{
		if (__waitFor == nullptr)
			return MoveNext();
		else
		{
			bool done = __waitFor->Next();
			if (done)
			{
				__waitFor = nullptr;
			}
			return false;
		}
	}

	inline void RunAll()
	{
		while (!MoveNext());
	}
};
