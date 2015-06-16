
#pragma once


#include <boost/asio.hpp>


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

	template <typename Buffer2, typename T2>
	BufferIterator(const BufferIterator<Buffer2, T2>& other)
		: _buffer(other._buffer),
		  _it(other._it)
	{

	}

	reference operator*() const {
		if (!_it) {
			throw std::range_error("BufferIterator::operator*");
		}
		return *_it;
	}

	pointer operator->() const {
		if (!_it) {
			throw std::range_error("BufferIterator::operator->");
		}
		return _it;
	}

	BufferIterator& operator++() {
		if (!_it) {
			throw std::range_error("BufferIterator::operator++");
		}
		_it = _buffer->move(_it, 1);
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
		if (!_it) {
			throw std::range_error("BufferIterator::operator++");
		}
		if (_it == _buffer->_first && _it == _buffer->_last) {
			if (distance > static_cast<difference_type>(_buffer->usefulDataSize())) {
				throw std::range_error("BufferIterator::operator++");
			}
		} else {
			if (distance > _buffer->distance(_it, _buffer->_last)) {
				throw std::range_error("BufferIterator::operator++");
			}
		}
		_it = _buffer->move(_it, distance);
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

	template <typename From>
	difference_type operator-(const From& from) const {
		const BufferIterator& to = *this;

		if (from._it != nullptr && to._it != nullptr)
		{
			return _buffer->distance(from._it, to._it);
		}
		if (from._it == nullptr && to._it == nullptr) {
			return 0;
		}
		if (from._it == _buffer->_first && from._it == _buffer->_last)
		{
			// буфер забит битком, ситуация end() - begin()
			return static_cast<difference_type>(_buffer->usefulDataSize());
		}
		if (to._it == nullptr)
		{
			return _buffer->distance(from._it, _buffer->_last);
		}

		throw std::logic_error("BufferIterator::operator-");
	}

public: //< Для конверсии Iterator -> ConstIterator
	Buffer* _buffer;
	pointer _it;
};


class Buffer {
public:
	enum { DefaultSize = 1024, MaximumSize = 1024 * 1024 };

	typedef BufferIterator<Buffer, uint8_t> Iterator;
	typedef BufferIterator<const Buffer, const uint8_t> ConstIterator;
	friend Iterator;
	friend ConstIterator;

	Buffer(size_t size = DefaultSize);

	Buffer(const std::initializer_list<uint8_t>& data);
	Buffer(const std::vector<uint8_t>& data);
	Buffer(const std::string& data);

	template <typename T>
	Buffer(T first, T last): Buffer(last - first) {
		pushBack(first, last);
	}

	Buffer(const Buffer& other) = delete;
	Buffer& operator=(const Buffer& other) = delete;
	Buffer(Buffer&& other);
	Buffer& operator=(Buffer&& other);

	~Buffer();

	void assign(const std::initializer_list<uint8_t>& data);
	void assign(const std::vector<uint8_t>& data);
	void assign(const std::string& data);

	template <typename T>
	void assign(T first, T last) {
		clear();
		pushBack(first, last);
	}

	void clear(size_t startIndex = 0);

	// Итераторы для usefulData
	Iterator begin();
	Iterator end();
	ConstIterator begin() const;
	ConstIterator end() const;

	uint8_t& front();
	uint8_t& back();

	std::vector<boost::asio::const_buffer> usefulData() const;
	size_t usefulDataSize() const;
	bool isUsefulDataContinuous() const;

	std::vector<boost::asio::mutable_buffer> freeSpace();
	size_t freeSpaceSize() const;
	size_t freeSpaceSizeAtTheBegining() const;
	size_t freeSpaceSizeAtTheEnd() const;

	void popFront(size_t size);
	void popBack(size_t size);
	void pushFront(size_t size);
	void pushBack(size_t size);

	void popFront(const Iterator& it);
	void popBack(const Iterator& it);
	void popFront(const ConstIterator& it);
	void popBack(const ConstIterator& it);

	template <typename T>
	void pushFront(T first, T last) {
		pushFront(last - first);
		Iterator pos(this, _first);
		std::copy(first, last, pos);
	}

	template <typename T>
	void pushBack(T first, T last) {
		auto size = last - first;
		pushBack(size);
		Iterator pos(this, move(_last, -size));
		std::copy(first, last, pos);
	}

	size_t size() const;

	bool operator==(const Buffer& other) const;

	uint8_t* getPointer(ptrdiff_t offset) {
		return move(_first, offset);
	}

	void reserve(size_t minimum);

private:
	uint8_t* move(const uint8_t* it, ptrdiff_t distance) const;
	ptrdiff_t distance(const uint8_t* a, const uint8_t* b) const;
	void realloc(size_t minimum);

private:
	uint8_t *_begin, *_end;
	size_t _usefulDataSize;
	uint8_t *_first, *_last;
};


class BufferIteratorRange {
public:
	BufferIteratorRange();
	BufferIteratorRange(const Buffer::ConstIterator& begin, const Buffer::ConstIterator& end);

	Buffer::ConstIterator begin() const;
	Buffer::ConstIterator end() const;
	size_t length() const;

	operator std::vector<uint8_t>() const;

	bool operator==(const std::vector<uint8_t>& other) const;
	bool operator!=(const std::vector<uint8_t>& other) const;

private:
	Buffer::ConstIterator _begin, _end;
};


typedef std::unique_ptr<Buffer, std::function<void(Buffer*)>&> BufferUniquePtr;

BufferUniquePtr MallocBuffer();
BufferUniquePtr MallocBuffer(size_t size);
BufferUniquePtr MallocBuffer(const std::initializer_list<uint8_t>& data);
