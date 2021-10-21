#pragma once

#include <functional>
#include <vector>
#include <memory>

#include <cstdio>

#define NAMEOF(CLASS) #CLASS

#define CRT_START()  virtual bool MoveNext() override { switch(__iter) { case 0:  

#define CRT_END() return true; }}

#define CRT_YIELD()  __iter = __LINE__ ; return  false;  case __LINE__: {}

#define CRT_WAIT_FOR(CRT) __waitFor = CRT; CRT_YIELD()

#define CRT_BREAK() return true;

#define CRT_RETURN(RESULT) __result = RESULT; CRT_BREAK()

class CoroutineImpl;
class CorroutineRImpl;

using Coroutine = std::shared_ptr<CoroutineImpl>;


class CoroutineImpl {
protected:
	int __iter = 0;

	Coroutine __waitFor = nullptr;

	virtual bool MoveNext() = 0;

	friend class CorroutineRImpl;
public:

	CoroutineImpl() {}
	CoroutineImpl(const CoroutineImpl&) = delete;
	CoroutineImpl& operator=(const CoroutineImpl&) = delete;

	// Returns TRUE when coroutine is done.
	bool Next()
	{
		if (!__waitFor)
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

template<class TResult>
class CoroutineRImpl :  public virtual CoroutineImpl {
protected:
	TResult __result = {};
public:
	CoroutineRImpl() : CoroutineImpl() {}
	CoroutineRImpl(const CoroutineRImpl&) = delete;
	CoroutineRImpl& operator=(const CoroutineRImpl&) = delete;

	TResult GetResult() const { printf("GetResult: 0x%p", (void*)__result); return __result; }
};


template <typename TResult>
using CoroutineR = std::shared_ptr<CoroutineRImpl<TResult>>;