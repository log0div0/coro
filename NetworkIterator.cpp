
#include "NetworkIterator.h"


NetworkIterator::NetworkIterator()
	: _socket(nullptr),
	  _buffer(nullptr),
	  _it(nullptr)
{

}

NetworkIterator::NetworkIterator(TcpSocket& socket, Buffer& buffer)
	: NetworkIterator(socket, buffer.begin())
{

}

NetworkIterator::NetworkIterator(TcpSocket& socket, const Buffer::Iterator& iterator)
	: _socket(&socket),
	  _buffer(iterator._buffer),
	  _it(iterator._it)
{
	if (!_it) {
		_it = _buffer->_last;
	}
}


NetworkIterator::reference NetworkIterator::operator*() const {
	while (!_buffer->isValid(_it)) {
		_socket->receiveSomeData(_buffer);
	}
	return *_it;
}

NetworkIterator::pointer NetworkIterator::operator->() const {
	return _it;
}


NetworkIterator& NetworkIterator::operator++() {
	_it = _buffer->moveForward(_it, 1);
	return *this;
}

NetworkIterator NetworkIterator::operator++(int) {
	NetworkIterator copy(*this);
	++*this;
	return copy;
}

NetworkIterator NetworkIterator::operator+(difference_type distance) const {
	NetworkIterator result(*_socket, *_buffer);
	result._it = _buffer->moveForward(_it, distance);
	return result;
}


void NetworkIterator::operator=(const NetworkIterator& other) {
	_socket = other._socket;
	_buffer = other._buffer;
	_it = other._it;
}

bool NetworkIterator::operator!=(const NetworkIterator& other) const {
	return _it != other._it;
}

bool NetworkIterator::operator==(const NetworkIterator& other) const {
	return _it == other._it;
}

NetworkIterator::difference_type NetworkIterator::operator-(const NetworkIterator& other) const {
	return _buffer->distance(other._it, _it);
}
