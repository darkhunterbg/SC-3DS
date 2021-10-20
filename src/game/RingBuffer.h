#pragma once

#include "Debug.h"

template <class T, unsigned SIZE>
class RingBuffer  {
private:
	unsigned _start = 0;
	unsigned _end = 0;
	T _buffer[SIZE];

public:
	RingBuffer(const RingBuffer&) = delete;
	RingBuffer& operator=(const RingBuffer&) = delete;

	RingBuffer()
	{
		static_assert(SIZE > 0, "RingBuffer SIZE must be greater than 0");
	}
	~RingBuffer() {}

	unsigned Enqueue(const T& element)
	{
		_buffer[_end] = element;

		++_end;
		if (_end >= SIZE)
		{
			_end = 0;
		}

		GAME_ASSERT(_end != _start, "Ring buffer overflow!");

		return _end;
	}
	bool TryDequeue(T* out)
	{
		bool success = true;

		if (_start == _end)
		{
			success = false;
		}
		else
		{
			*out = _buffer[_start++];
			if (_start >= SIZE)
				_start = 0;
		}

		return success;
	}

	unsigned Size() const
	{
		if (_start > _end)
		{
			return SIZE - _start + _end;
		}
		return  _end - _start;
	}
	constexpr unsigned Capacity() const
	{
		return SIZE;
	}
	bool Empty() const
	{
		return Size() == 0;
	}
};
