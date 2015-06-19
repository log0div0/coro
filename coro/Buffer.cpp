
#include "coro/Buffer.h"
#include "coro/ObjectFactory.h"

BufferIteratorRange::BufferIteratorRange() {}

BufferIteratorRange::BufferIteratorRange(const Buffer::ConstIterator& begin, const Buffer::ConstIterator& end)
	: _begin(begin), _end(end) {}

Buffer::ConstIterator BufferIteratorRange::begin() const {
	return _begin;
}

Buffer::ConstIterator BufferIteratorRange::end() const {
	return _end;
}

size_t BufferIteratorRange::length() const {
	return _end - _begin;
}

BufferIteratorRange::operator std::vector<uint8_t>() const {
	return { _begin, _end };
}

bool BufferIteratorRange::operator==(const std::vector<uint8_t>& other) const {
	if (static_cast<ptrdiff_t>(other.size()) != _end - _begin) {
		return false;
	}
	if (!other.size()) {
		return true;
	}
	return std::equal(_begin, _end, other.begin());
}

bool BufferIteratorRange::operator!=(const std::vector<uint8_t>& other) const {
	return !(*this == other);
}



BufferUniquePtr MallocBuffer() {
	auto buffer = ObjectFactory<Buffer>::make();
	buffer->clear();
	return buffer;
}

BufferUniquePtr MallocBuffer(size_t size) {
	auto buffer = MallocBuffer();
	buffer->reserve(size);
	return buffer;
}

BufferUniquePtr MallocBuffer(const std::initializer_list<uint8_t>& data) {
	auto buffer = ObjectFactory<Buffer>::make();
	buffer->assign(data);
	return buffer;
}
