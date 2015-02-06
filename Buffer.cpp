
#include "Buffer.h"

Buffer::Buffer(const std::initializer_list<uint8_t>& list)
	: Buffer(list.size())
{
	std::copy(list.begin(), list.end(), _begin);
	_usefulDataSize = list.size();
}

Buffer::Buffer(size_t size)
	: _begin(new uint8_t[size]),
	  _end(_begin + size),
	  _usefulDataSize(0),
	  _first(_begin),
	  _last(_begin)
{

}

Buffer::~Buffer()
{
	delete[] _begin;
}

Buffer::Iterator Buffer::begin() {
	return Iterator(this, _first);
}

Buffer::Iterator Buffer::end() {
	return Iterator(this, 0);
}

Buffer::ConstIterator Buffer::begin() const {
	return ConstIterator(this, _first);
}

Buffer::ConstIterator Buffer::end() const {
	return ConstIterator(this, 0);

}

std::vector<boost::asio::const_buffer> Buffer::usefulData() const {
	if ( (_first <= _last) && (usefulDataSize() != size()) ) {
		return {
			{ _first, static_cast<size_t>(_last - _first) }
		};
	} else {
		return {
			{ _first, static_cast<size_t>(_end - _first) },
			{ _begin, static_cast<size_t>(_first - _begin) }
		};
	}
}

size_t Buffer::usefulDataSize() const {
	return _usefulDataSize;
}

std::vector<boost::asio::mutable_buffer> Buffer::freeSpace() {
	if ( (_first <= _last) && (usefulDataSize() != size()) ) {
		return {
			{ _last, static_cast<size_t>(_end - _last) },
			{ _begin, static_cast<size_t>(_first - _begin) }
		};
	} else {
		return {
			{ _last, static_cast<size_t>(_first - _last) }
		};
	}
}

size_t Buffer::freeSpaceSize() const {
	return size() - _usefulDataSize;
}

void Buffer::reduceFront(size_t size) {
	if (usefulDataSize() < size) {
		throw std::out_of_range("Buffer::reduceFront");
	}
	_usefulDataSize -= size;
	_first = moveForward(_first, size);
}

void Buffer::reduceBack(size_t size) {
	if (usefulDataSize() < size) {
		throw std::out_of_range("Buffer::reduceBack");
	}
	_usefulDataSize -= size;
	_last = moveBackward(_last, size);
}

void Buffer::expandFront(size_t size) {
	if (freeSpaceSize() < size) {
		throw std::out_of_range("Buffer::expandFront");
	}
	_usefulDataSize += size;
	_first = moveBackward(_first, size);
}

void Buffer::expandBack(size_t size) {
	if (freeSpaceSize() < size) {
		throw std::out_of_range("Buffer::expandBack");
	}
	_usefulDataSize += size;
	_last = moveForward(_last, size);
}


size_t Buffer::size() const {
	return _end - _begin;
}

bool Buffer::operator==(const Buffer& other) const {
	if (this == &other) {
		return true;
	}
	if (usefulDataSize() != other.usefulDataSize()) {
		return false;
	}
	return std::equal(begin(), end(), other.begin());
}

