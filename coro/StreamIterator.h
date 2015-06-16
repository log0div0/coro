
#pragma once

#include "coro/Buffer.h"

template <typename Stream>
class StreamIterator {
public:
	typedef ptrdiff_t difference_type;
	typedef uint8_t value_type;
	typedef uint8_t* pointer;
	typedef uint8_t& reference;
	typedef std::forward_iterator_tag iterator_category;

	StreamIterator()
		: _stream(nullptr),
		  _buffer(nullptr),
		  _offset(-1)
	{

	}

	StreamIterator(Stream& handle, Buffer& buffer)
		: _stream(&handle),
		  _buffer(&buffer),
		  _offset(0)
	{
	}

	reference operator*() {
		read();
		return *_buffer->getPointer(_offset);
	}

	pointer operator->() {
		read();
		return _buffer->getPointer(_offset);
	}

	StreamIterator& operator++() {
		if (static_cast<size_t>(_offset + 1) > _buffer->size()) {
			throw std::range_error("StreamIterator::operator++");
		}
		++_offset;
		return *this;
	}

	StreamIterator operator++(int) {
		StreamIterator copy(*this);
		++*this;
		return copy;
	}

	StreamIterator& operator+=(difference_type distance) {
		if (static_cast<size_t>(_offset + distance) > _buffer->size()) {
			throw std::range_error("StreamIterator::operator+=");
		}
		_offset += distance;
		return *this;
	}

	StreamIterator operator+(difference_type distance) const {
		return StreamIterator(*this) += distance;
	}

	void operator=(const StreamIterator& other) {
		_stream = other._stream;
		_buffer = other._buffer;
		_offset = other._offset;
	}

	bool operator!=(const StreamIterator& other) const {
		return _offset != other._offset;
	}

	bool operator==(const StreamIterator& other) const {
		return _offset == other._offset;
	}

	difference_type operator-(const StreamIterator& other) const {
		return other._offset - _offset;
	}

	operator Buffer::Iterator() const {
		return _buffer->begin() + _offset;
	}

private:
	void read() {
		while (static_cast<size_t>(_offset) >= _buffer->usefulDataSize()) {
			_buffer->pushBack(_stream->readSome(_buffer));
		}
	}

	Stream* _stream;
	Buffer* _buffer;
	difference_type _offset;
};

