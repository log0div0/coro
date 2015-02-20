
#include "NetworkIterator.h"


NetworkIterator::NetworkIterator()
	: _socket(nullptr),
	  _buffer(nullptr),
	  _offset(0)
{

}

NetworkIterator::NetworkIterator(TcpSocket& socket, Buffer& buffer)
	: NetworkIterator(socket, buffer.end())
{

}

NetworkIterator::NetworkIterator(TcpSocket& socket, const Buffer::Iterator& iterator)
	: _socket(&socket),
	  _buffer(iterator._buffer),
	  _offset(iterator - _buffer->begin())
{
}


NetworkIterator::reference NetworkIterator::operator*() const {
	pointer it = getPointer();
	while (!_buffer->isValid(it)) {
		_socket->receiveSomeData(_buffer);
	}
	return *it;
}

NetworkIterator::pointer NetworkIterator::operator->() const {
	return getPointer();
}


NetworkIterator& NetworkIterator::operator++() {
	if (static_cast<size_t>(_offset + 1) > _buffer->size()) {
		throw std::range_error("NetworkIterator::operator++");
	}
	++_offset;
	return *this;
}

NetworkIterator NetworkIterator::operator++(int) {
	NetworkIterator copy(*this);
	++*this;
	return copy;
}

NetworkIterator NetworkIterator::operator+(difference_type distance) const {
	if (static_cast<size_t>(_offset + distance) > _buffer->size()) {
		throw std::range_error("NetworkIterator::operator+");
	}
	NetworkIterator result(*this);
	result._offset += distance;
	return result;
}


void NetworkIterator::operator=(const NetworkIterator& other) {
	_socket = other._socket;
	_buffer = other._buffer;
	_offset = other._offset;
}

bool NetworkIterator::operator!=(const NetworkIterator& other) const {
	return _offset != other._offset;
}

bool NetworkIterator::operator==(const NetworkIterator& other) const {
	return _offset == other._offset;
}

NetworkIterator::difference_type NetworkIterator::operator-(const NetworkIterator& other) const {
	return other._offset - _offset;
}

NetworkIterator::pointer NetworkIterator::getPointer() const {
	return _buffer->moveForward(_buffer->_first, _offset);
}
