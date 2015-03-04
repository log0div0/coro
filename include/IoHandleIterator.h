
#pragma once

#include "Buffer.h"

template <typename Handle>
class IoHandleIterator {
public:
	typedef ptrdiff_t difference_type;
	typedef uint8_t value_type;
	typedef uint8_t* pointer;
	typedef uint8_t& reference;
	typedef std::forward_iterator_tag iterator_category;

	IoHandleIterator()
		: _handle(nullptr),
		  _buffer(nullptr),
		  _offset(0)
	{

	}

	IoHandleIterator(Handle& handle, Buffer& buffer)
		: _handle(&handle),
		  _buffer(&buffer),
		  _offset(0)
	{
	}

	reference operator*() {
		receiveData();
		return *_buffer->getPointer(_offset);
	}

	pointer operator->() {
		receiveData();
		return _buffer->getPointer(_offset);
	}

	IoHandleIterator& operator++() {
		if (static_cast<size_t>(_offset + 1) > _buffer->size()) {
			throw std::range_error("IoHandleIterator::operator++");
		}
		++_offset;
		return *this;
	}

	IoHandleIterator operator++(int) {
		IoHandleIterator copy(*this);
		++*this;
		return copy;
	}

	IoHandleIterator& operator+=(difference_type distance) {
		if (static_cast<size_t>(_offset + distance) > _buffer->size()) {
			throw std::range_error("IoHandleIterator::operator+=");
		}
		_offset += distance;
		return *this;
	}

	IoHandleIterator operator+(difference_type distance) const {
		return IoHandleIterator(*this) += distance;
	}

	void operator=(const IoHandleIterator& other) {
		_handle = other._handle;
		_buffer = other._buffer;
		_offset = other._offset;
	}

	bool operator!=(const IoHandleIterator& other) const {
		return (_handle != other._handle) || (_offset != other._offset);
	}

	bool operator==(const IoHandleIterator& other) const {
		return (_handle == other._handle) && (_offset == other._offset);
	}

	difference_type operator-(const IoHandleIterator& other) const {
		return other._offset - _offset;
	}

	operator Buffer::Iterator() const {
		return _buffer->begin() + _offset;
	}

private:
	void receiveData() {
		while (static_cast<size_t>(_offset) >= _buffer->usefulDataSize()) {
			_handle->receiveData(_buffer);
		}
	}

	Handle* _handle;
	Buffer* _buffer;
	difference_type _offset;
};

