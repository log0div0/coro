
#pragma once

#include <boost/asio.hpp>

class Buffer;

template <typename Buffer, typename T>
class BufferIterator {
public:
	typedef size_t difference_type;
	typedef T value_type;
	typedef T* pointer;
	typedef T& reference;
	typedef std::random_access_iterator_tag iterator_category;

	BufferIterator(const Buffer* buffer, pointer it)
		: _buffer(buffer),
		  _it(it)
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
			_it = 0;
		}
		return *this;
	}

	BufferIterator operator++(int) {
		BufferIterator copy(*this);
		++*this;
		return copy;
	}

	BufferIterator& operator--() {
		if (_it == _buffer->_first) {
			throw std::out_of_range("BufferIterator decrement");
		}
		_it = _buffer->moveBackward(_it, 1);
		return *this;
	}

	BufferIterator operator--(int) {
		BufferIterator copy(*this);
		--*this;
		return copy;
	}

	bool operator!=(const BufferIterator& other) const {
		return _it != other._it;
	}

private:
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

	Buffer(const std::initializer_list<uint8_t>& list);
	explicit Buffer(size_t size);
	~Buffer();

	// Итераторы для usefulData
	Iterator begin();
	Iterator end();
	ConstIterator begin() const;
	ConstIterator end() const;

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

	size_t size() const;

	bool operator==(const Buffer& other) const;

private:
	template <typename T>
	T moveForward(T it, size_t distance) const {
		return _begin + (it + distance - _begin) % size();
	}

	template <typename T>
	T moveBackward(T it, size_t distance) const {
		return _begin + (it - distance - _begin) % size();
	}

private:
	uint8_t *_begin, *_end;
	size_t _usefulDataSize;
	uint8_t *_first, *_last;
};
