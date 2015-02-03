
#include "Buffer.h"

Buffer::Buffer(uint8_t* data, size_t size)
	: _data(data), _size(size) {
}

Buffer::Buffer(const Buffer& other)
	: _data(other._data),
	  _size(other._size)
{

}

boost::asio::const_buffers_1 Buffer::asio() const {
	return boost::asio::buffer((const uint8_t*)_data, _size);
}

boost::asio::mutable_buffers_1 Buffer::asio() {
	return boost::asio::buffer(_data, _size);
}

uint8_t* Buffer::data() const {
	return _data;
}

size_t Buffer::size() const {
	return _size;
}

void Buffer::truncate(size_t size) {
	assert(size <= _size);
	_size = size;
}

bool Buffer::isEqual(const Buffer& other) {
	return (_size == other._size) && !memcmp(_data, other._data, _size);
}




DynamicBuffer::DynamicBuffer(std::initializer_list<uint8_t> list)
	: Buffer(new uint8_t[list.size()], list.size())
{
	std::copy(list.begin(), list.end(), _data);
}

DynamicBuffer::DynamicBuffer(size_t size)
	: Buffer(new uint8_t[size], size)
{

}

DynamicBuffer::~DynamicBuffer() {
	delete[] _data;
}
