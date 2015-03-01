
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

	BufferIterator(Buffer* buffer, pointer it)
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

	BufferIterator& operator+=(difference_type distance) {
		_it = _buffer->moveForward(_it, distance);
		if (_it == _buffer->_last) {
			_it = nullptr;
		}
		return *this;
	}

	BufferIterator operator+(difference_type distance) const {
		return BufferIterator(*this) += distance;
	}

	void operator=(const BufferIterator& other) {
		_buffer = other._buffer;
		_it = other._it;
	}

	bool operator!=(const BufferIterator& other) const {
		return _it != other._it;
	}

	bool operator==(const BufferIterator& other) const {
		return _it == other._it;
	}

	difference_type operator-(const BufferIterator& other) const {
		if (_it != nullptr) {
			return _buffer->distance(other._it, _it);
		} else {
			if (other._it == nullptr) {
				return 0;
			}
			if (other._it != _buffer->_last) {
				return _buffer->distance(other._it, _buffer->_last);
			} else {
				return _buffer->usefulDataSize();
			}
		}
	}

public: //< Для конверсии Iterator -> ConstIterator
	Buffer* _buffer;
	pointer _it;
};


class Buffer {
public:
	typedef BufferIterator<Buffer, uint8_t> Iterator;
	typedef BufferIterator<const Buffer, const uint8_t> ConstIterator;
	friend Iterator;
	friend ConstIterator;
	friend class NetworkIterator;

	explicit Buffer(size_t size);

	Buffer(const std::initializer_list<uint8_t>& list);
	Buffer(const std::string& data);
	Buffer(const std::vector<uint8_t>& data);

	Buffer(const Buffer& other) = delete;
	Buffer& operator=(const Buffer& other) = delete;
	Buffer(Buffer&& other) = delete;
	Buffer& operator=(Buffer&& other) = delete;

	~Buffer();

	void clear();

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

	void popFront(size_t size);
	void popBack(size_t size);
	void pushFront(size_t size);
	void pushBack(size_t size);

	void popFront(const Iterator& it);

	template <typename T>
	void pushFront(T begin, T end) {
		pushFront(end - begin);
		Iterator first(this, _first);
		std::copy(begin, end, first);
	}

	template <typename T>
	void pushBack(T begin, T end) {
		Iterator last(this, _last);
		pushBack(end - begin);
		std::copy(begin, end, last);
	}

	template <typename T>
	void assign(T begin, T end) {
		clear();
		pushBack(begin, end);
	}

	size_t size() const;

	bool operator==(const Buffer& other) const;

private:

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

	template <typename T>
	bool isValid(T it) const {
		if (usefulDataSize() == 0) {
			return false;
		}
		if (_first <= _last) {
			return (it >= _first) && (it < _last);
		} else {
			return
				( (it >= _first) && (it < _end) ) ||
				( (it >= _begin) && (it < _last) );
		}
	}

private:
	uint8_t *_begin, *_end;
	size_t _usefulDataSize;
	uint8_t *_first, *_last;
};
