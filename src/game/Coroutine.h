#pragma once

#include <functional>
#include <vector>

class Coroutine {
private:
	std::vector<std::function<bool()>> _iterations;
	bool _done = false;
	int iter = 0;
public:
	Coroutine& AddNext(std::function<bool()> i);

	Coroutine(const Coroutine&) = delete;
	Coroutine& operator=(const Coroutine&) = delete;

	Coroutine();

	bool MoveNext();
};