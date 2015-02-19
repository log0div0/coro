
#pragma once

#include <boost/asio.hpp>

class Buffer;

template <typename Buffer, typename T>
class BufferIterator {
public:
	typedef ptrdiff_t difference_type;
	typedef T value_type;
	typedef T* pointer;
	typedef T& reference;
	typedef std::forward_iterator_tag iterator_category;

	BufferIterator()
		: _buffer(nullptr),
		  _it(nullptr) {

	}

	BufferIterator(const Buffer* buffer, pointer it)
		: _buffer(buffer),
		  _it(it)
	{

	}

	template <typename U>
	BufferIterator(const BufferIterator<Buffer, U>& other)
		: _buffer(other._buffer),
		  _it(other._it)
	{

	}

	reference operator*() const {
		return *_it;
	}

	pointer operator->() const {
		return _it;
	}

	BufferIterator& operator++() {
		_it = _buffer->moveForward(_it, 1);
		if (_it == _buffer->_last) {
			_it = nullptr;
		}
		return *this;
	}

	BufferIterator operator++(int) {
		BufferIterator copy(*this);
		++*this;
		return copy;
	}

	bool operator!=(const BufferIterator& other) const {
		return _it != other._it;
	}

	bool operator==(const BufferIterator& other) const {
		return _it == other._it;
	}

	void operator=(const BufferIterator& other) {
		_buffer = other._buffer;
		_it = other._it;
	}

	BufferIterator operator+(size_t size) const {
		return BufferIterator(_buffer, _buffer->moveForward(_it, size));
	}

	difference_type operator-(const BufferIterator& other) const {
		if (_it != 0) {
			return _buffer->distance(other._it, _it);
		} else {
			if (other._it != _buffer->_last) {
				return _buffer->distance(other._it, _buffer->_last);
			} else {
				return _buffer->usefulDataSize();
			}
		}
	}

public: //< Для конверсии Iterator -> ConstIterator
	const Buffer* _buffer;
	pointer _it;
};

// Циклический буфер
// boost::circular_buffer не даёт нам доступа к неиспользуемой памяти. Можно было бы выделить сразу
// всю память целиком и итераторами обозначить границы "заполненной" и "пустой" частей. НО
// возникает проблема: как только итератор достигнет позиции, которую boost::circular_buffer
// считает концом полезной нагрузки - итератор обнуляется. Вариант второй: добраться до кишочков
// boost::circular_buffer c помощью &front(). НО велосипед лучше костыля.
class Buffer {
public:
	typedef BufferIterator<Buffer, uint8_t> Iterator;
	typedef BufferIterator<Buffer, const uint8_t> ConstIterator;
	friend Iterator;
	friend ConstIterator;

	explicit Buffer(size_t size);
	Buffer(const std::initializer_list<uint8_t>& list);
	Buffer(const std::string& data);
	Buffer(const std::vector<uint8_t>& data);
	Buffer(Buffer&& other);
	~Buffer();

	// Итераторы для usefulData
	Iterator begin();
	Iterator end();
	ConstIterator begin() const;
	ConstIterator end() const;

	uint8_t& front();
	uint8_t& back();

	std::vector<boost::asio::const_buffer> usefulData() const;
	size_t usefulDataSize() const;
	std::vector<boost::asio::mutable_buffer> freeSpace();
	size_t freeSpaceSize() const;

	// Первые size байт usefulData переходят в freeSpace
	void reduceFront(size_t size);
	// Последние size байт usefulData переходят в freeSpace
	void reduceBack(size_t size);

	// Граница usefulData расширяется в начале за счёт freeSpace
	void expandFront(size_t size);
	// Граница usefulData расширяется в конце за счёт freeSpace
	void expandBack(size_t size);

	template <typename T>
	void expandBack(T begin, T end) {
		Iterator last(this, _last);
		expandBack(end - begin);
		std::copy(begin, end, last);
	}

	size_t size() const;

	bool operator==(const Buffer& other) const;

private:
	Buffer(const Buffer& other);

	template <typename T>
	T moveForward(T it, size_t distance) const {
		return _begin + (it - _begin + distance) % size();
	}

	template <typename T>
	T moveBackward(T it, size_t distance) const {
		return _begin + (it - _begin - distance + size()) % int64_t(size());
	}

	template <typename T>
	size_t distance(T a, T b) const {
		if (a <= b) {
			return b - a;
		} else {
			return (_end - a) + (b - _first);
		}
	}

private:
	uint8_t *_begin, *_end;
	size_t _usefulDataSize;
	uint8_t *_first, *_last;
};
