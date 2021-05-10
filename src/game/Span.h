#pragma once

template<class T>
class Span {
public:
	class ConstIterator {
	public:
		ConstIterator(const Span<T>& span, unsigned pos) : _span(span), _pos(pos) {}
		const T& operator* () const { return _span[_pos]; }
		bool operator != (const ConstIterator& other) const { return _pos != other._pos; }
		const ConstIterator& operator++ () { ++_pos;	return *this; }
	private:
		const Span<T> _span;
		unsigned _pos;
	};

	class Iterator {
	public:
		Iterator( Span<T>& span, unsigned pos) : _span(span), _pos(pos) {}
		T& operator* () { return _span[_pos]; }
		bool operator != (const Iterator& other) const { return _pos != other._pos; }
		Iterator& operator++ () { ++_pos;	return *this; }
	private:
		Span<T> _span;
		unsigned _pos;
	};

public:
	Span() {}

	Span(const T* data, unsigned size) : _data(const_cast<T*>(data)), _size(size) {
	}


	Span(T* data, unsigned size) : _data(data), _size(size) {
	}

	inline unsigned Size() const {
		return _size;
	}
	T* Data() {
		return _data;
	}
	const T* Data() const {
		return _data;
	}
	const T& At(unsigned index) const {
		//AIO_ASSERT(_data, "Tried to access nullptr span.");
		//AIO_ASSERT(index < _size, "Out of range access %i of %i.", index, _size);

		return _data[index];
	}

	T& At(unsigned index) {
		//AIO_ASSERT(_data, "Tried to access nullptr span.");
		//AIO_ASSERT(index < _size, "Out of range access %i of %i.", index, _size);

		return _data[index];
	}

	inline const T& operator[](unsigned index)  const {
		return At(index);
	}
	ConstIterator begin() const {
		return ConstIterator(*this, 0);
	}
	ConstIterator end() const {
		return ConstIterator(*this, _size);
	}

	Iterator begin() {
		return Iterator(*this, 0);
	}
	Iterator end() {
		return Iterator(*this, _size);
	}
private:

	T* _data = nullptr;
	unsigned _size = 0;
};