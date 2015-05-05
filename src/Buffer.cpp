
#include "Buffer.h"
#include "Malloc.h"

Buffer::Buffer(size_t size)
	: _begin(new uint8_t[size]),
	  _end(_begin + size),
	  _usefulDataSize(0),
	  _first(_begin),
	  _last(_begin)
{

}

Buffer::Buffer(const std::initializer_list<uint8_t>& data): Buffer(data.begin(), data.end()) {

}

Buffer::Buffer(const std::vector<uint8_t>& data): Buffer(data.begin(), data.end()) {

}

Buffer::Buffer(const std::string& data): Buffer(data.begin(), data.end()) {

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

Buffer& Buffer::operator=(Buffer&& other) {
	std::swap(_begin, other._begin);
	std::swap(_end, other._end);
	std::swap(_usefulDataSize, other._usefulDataSize);
	std::swap(_first, other._first);
	std::swap(_last, other._last);
	return *this;
}

Buffer::~Buffer()
{
	delete[] _begin;
}

void Buffer::assign(const std::initializer_list<uint8_t>& data) {
	assign(data.begin(), data.end());
}

void Buffer::assign(const std::vector<uint8_t>& data) {
	assign(data.begin(), data.end());
}

void Buffer::assign(const std::string& data) {
	assign(data.begin(), data.end());
}

void Buffer::clear(size_t startIndex) {
	_first = _last = move(_begin, startIndex);
	_usefulDataSize = 0;
}

Buffer::Iterator Buffer::begin() {
	return Iterator(this, _usefulDataSize ? _first : nullptr);
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
	return *move(_first, _usefulDataSize - 1);
}

std::vector<boost::asio::const_buffer> Buffer::usefulData() const {
	if ( (_first <= _last) && (usefulDataSize() != size()) ) {
		return {
			{ _first, static_cast<size_t>(_last - _first) }
		};
	} else {
		return {
			{ _first, static_cast<size_t>(_end - _first) },
			{ _begin, static_cast<size_t>(_last - _begin) }
		};
	}
}

size_t Buffer::usefulDataSize() const {
	return _usefulDataSize;
}

bool Buffer::isUsefulDataContinuous() const {
	return (_first == _begin) || (_last == _begin) || (_first <= _last);
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

size_t Buffer::freeSpaceSizeAtTheBegining() const {
	if (!isUsefulDataContinuous()) {
		return 0;
	}
	if (_first == _begin && _last == _begin) {
		return size() - usefulDataSize();
	}
	return _first - _begin;
}

size_t Buffer::freeSpaceSizeAtTheEnd() const {
	if (!isUsefulDataContinuous()) {
		return 0;
	}
	if (_first == _begin && _last == _begin) {
		return size() - usefulDataSize();
	}
	if (_last == _begin) {
		return 0;
	}
	return _end - _last;
}

void Buffer::popFront(size_t size) {
	if (usefulDataSize() < size) {
		throw std::range_error("Buffer::popFront");
	}
	_usefulDataSize -= size;
	_first = move(_first, size);
}

void Buffer::popBack(size_t size) {
	if (usefulDataSize() < size) {
		throw std::range_error("Buffer::popBack");
	}
	_usefulDataSize -= size;
	_last = move(_last, -size);
}

void Buffer::pushFront(size_t size) {
	if (freeSpaceSize() < size) {
		realloc(usefulDataSize() + size);
	}
	_usefulDataSize += size;
	_first = move(_first, -size);
}

void Buffer::pushBack(size_t size) {
	if (freeSpaceSize() < size) {
		realloc(usefulDataSize() + size);
	}
	_usefulDataSize += size;
	_last = move(_last, size);
}


void Buffer::popFront(const Iterator& it) {
	popFront(it - begin());
}


void Buffer::popBack(const Iterator& it) {
	popBack(end() - it);
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
	if (!usefulDataSize()) {
		return true;
	}
	return std::equal(begin(), end(), other.begin());
}

void Buffer::reserve(size_t minimum) {
	if (size() < minimum) {
		realloc(minimum);
	}
}

uint8_t* Buffer::move(const uint8_t* it, ptrdiff_t distance) const {
	ptrdiff_t position = ((it - _begin) + distance) % ptrdiff_t(size());
	if (position < 0) {
		position += size();
	}
	return _begin + position;
}

ptrdiff_t Buffer::distance(const uint8_t* a, const uint8_t* b) const {
	if (a <= b) {
		return b - a;
	} else {
		return (_end - a) + (b - _begin);
	}
}

void Buffer::realloc(size_t minimum) {
	size_t size = this->size();
	while (size <= minimum) {
		size *= 2;
		if (size > MaximumSize) {
			throw std::runtime_error("Buffer::realloc");
		}
	}
	uint8_t* data = new uint8_t[size];
	std::copy(begin(), end(), data);
	delete[] _begin;
	_begin = data;
	_end = data + size;
	_first = _begin;
	_last = _begin + _usefulDataSize;
}

BufferUniquePtr MallocBuffer() {
	auto buffer = Malloc<Buffer>();
	buffer->clear();
	return buffer;
}

BufferUniquePtr MallocBuffer(size_t size) {
	auto buffer = MallocBuffer();
	buffer->reserve(size);
	return buffer;
}

BufferUniquePtr MallocBuffer(const std::initializer_list<uint8_t>& data) {
	auto buffer = Malloc<Buffer>();
	buffer->assign(data);
	return buffer;
}
