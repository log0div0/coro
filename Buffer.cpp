
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

Buffer::Buffer(const std::string& data)
	: Buffer(data.size())
{
	std::copy(data.begin(), data.end(), _begin);
	_usefulDataSize = data.size();
}

Buffer::Buffer(const std::vector<uint8_t>& data)
	: Buffer(data.size())
{
	std::copy(data.begin(), data.end(), _begin);
	_usefulDataSize = data.size();
}

Buffer::Buffer(Buffer&& other)
	: _begin(other._begin),
	  _end(other._end),
	  _usefulDataSize(other._usefulDataSize),
	  _first(other._first),
	  _last(other._last)
{
	other._begin = nullptr;
	other._end = nullptr;
	other._usefulDataSize = 0;
	other._first = nullptr;
	other._last = nullptr;
}

Buffer::~Buffer()
{
	delete[] _begin;
}

Buffer::Iterator Buffer::begin() {
	return Iterator(this, _first);
}

Buffer::Iterator Buffer::end() {
	return Iterator(this, nullptr);
}

Buffer::ConstIterator Buffer::begin() const {
	return ConstIterator(this, _first);
}

Buffer::ConstIterator Buffer::end() const {
	return ConstIterator(this, nullptr);
}

uint8_t& Buffer::front() {
	assert(_usefulDataSize);
	return *_first;
}

uint8_t& Buffer::back() {
	assert(_usefulDataSize);
	return *moveForward(_first, _usefulDataSize - 1);
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

void Buffer::popFront(size_t size) {
	if (usefulDataSize() < size) {
		throw std::range_error("Buffer::popFront");
	}
	_usefulDataSize -= size;
	_first = moveForward(_first, size);
}

void Buffer::popBack(size_t size) {
	if (usefulDataSize() < size) {
		throw std::range_error("Buffer::popBack");
	}
	_usefulDataSize -= size;
	_last = moveBackward(_last, size);
}

void Buffer::pushFront(size_t size) {
	if (freeSpaceSize() < size) {
		throw std::range_error("Buffer::pushFront");
	}
	_usefulDataSize += size;
	_first = moveBackward(_first, size);
}

void Buffer::pushBack(size_t size) {
	if (freeSpaceSize() < size) {
		throw std::range_error("Buffer::pushBack");
	}
	_usefulDataSize += size;
	_last = moveForward(_last, size);
}


void Buffer::popFront(const Iterator& it) {
	popFront(it - begin());
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


