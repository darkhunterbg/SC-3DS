#include "Coroutine.h"

Coroutine& Coroutine::AddNext(std::function<bool()> i)
{
	_iterations.push_back(i);

	return *this;
}

Coroutine::Coroutine()
{
}

bool Coroutine::MoveNext()
{
	if (_done) return false;

	if (iter >= _iterations.size())
	{
		_done = true; 
		return false;
	}

	_done = !_iterations[iter++]();

	return true;
}
