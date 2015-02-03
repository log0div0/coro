
#pragma once

#include <boost/asio.hpp>

class Buffer {
public:
	Buffer(uint8_t* data, size_t size);
	Buffer(const Buffer& other);

	boost::asio::const_buffers_1 asio() const;
	boost::asio::mutable_buffers_1 asio();

	uint8_t* data() const;
	size_t size() const;

	void truncate(size_t size);

	bool isEqual(const Buffer& other);

protected:
	uint8_t* _data;
	size_t _size;
};


class DynamicBuffer: public Buffer {
public:
	DynamicBuffer(std::initializer_list<uint8_t> list);
	explicit DynamicBuffer(size_t size);
	~DynamicBuffer();

private:
	DynamicBuffer(const DynamicBuffer& other);
};

