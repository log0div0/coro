
#pragma once

#include "TcpSocket.h"

class NetworkIterator {
public:
	typedef ptrdiff_t difference_type;
	typedef uint8_t value_type;
	typedef uint8_t* pointer;
	typedef uint8_t& reference;
	typedef std::forward_iterator_tag iterator_category;

	NetworkIterator();
	NetworkIterator(TcpSocket& socket, Buffer& buffer);
	NetworkIterator(TcpSocket& socket, const Buffer::Iterator& iterator);

	reference operator*() const;
	pointer operator->() const;

	NetworkIterator& operator++();
	NetworkIterator operator++(int);
	NetworkIterator operator+(difference_type distance) const;

	void operator=(const NetworkIterator& other);
	bool operator!=(const NetworkIterator& other) const;
	bool operator==(const NetworkIterator& other) const;
	difference_type operator-(const NetworkIterator& other) const;

private:
	TcpSocket* _socket;
	Buffer* _buffer;
	pointer _it;
};
