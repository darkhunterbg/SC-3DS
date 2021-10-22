#pragma once

#include <functional>
#include <vector>
#include <memory>

#include <cstdio>

#define NAMEOF(CLASS) #CLASS

#define CRT_START()  virtual bool MoveNext() override { switch(__iter) { case 0:  

#define CRT_END() return true; } return true;}

#define CRT_YIELD()  __iter = __LINE__ ; return  false;  case __LINE__: {}

#define CRT_WAIT_FOR(CRT) __waitFor = CRT; CRT_YIELD()

#define CRT_BREAK() return true;

#define CRT_RETURN(RESULT) __result = RESULT; CRT_BREAK()

class CoroutineImpl;
class CorroutineRImpl;

using Coroutine = std::shared_ptr<CoroutineImpl>;


class CoroutineImpl {
private:
	bool _done = false;
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
		if (IsCompleted()) return true;
		
		if (!__waitFor)
			return _done = MoveNext();
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

	inline bool IsCompleted() const { return  _done; }

	inline void RunAll()
	{
		while (!Next());
	}
};


template<class TResult>
class CoroutineRImpl : public  CoroutineImpl {
protected:
	TResult __result = {};
public:
	CoroutineRImpl() : CoroutineImpl() {}
	CoroutineRImpl(const CoroutineRImpl&) = delete;
	CoroutineRImpl& operator=(const CoroutineRImpl&) = delete;

	TResult GetResult() const { return __result; }
};


template <typename TResult>
using CoroutineR = std::shared_ptr<CoroutineRImpl<TResult>>;


namespace hidden
{
	class GenericCrt : public CoroutineImpl {
		std::function<void()> _func;
	public:
		GenericCrt(std::function<void()> func) : _func(func) {}

		CRT_START()
		{
			_func();
		}
		CRT_END();
	};

	template<class TResult>
	class GenericRCrt : public CoroutineRImpl<TResult> {
		std::function<TResult()> _func;
	public:
		GenericRCrt(std::function<TResult()> func) : _func(func) {}

		virtual bool MoveNext() override
		{
			CoroutineRImpl<TResult>::__result = _func();
			return true;
		}
	};
}


static Coroutine CoroutineFromFunction(std::function<void()> func)
{
	return Coroutine(new hidden::GenericCrt(func));
}

template<class TResult>
static CoroutineR<TResult> CoroutineFromFunctionResult(std::function<TResult()> func)
{
	return CoroutineR<TResult>(new hidden::GenericRCrt<TResult>(func));
}