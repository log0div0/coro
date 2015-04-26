
#pragma once


#include "StreamIterator.h"
#include "AsioTask.h"


template <typename Handle>
class Stream {
public:
	typedef StreamIterator<Stream> Iterator;

	Stream(Handle handle): _handle(std::move(handle)) {}

	// VS2013 не умеет их генерить
	Stream(Stream&& other): _handle(std::move(other._handle)) {}

	Stream& operator=(Stream&& other) {
		_handle = std::move(other._handle);
		return *this;
	}

	Iterator iterator(Buffer& buffer) {
		return Iterator(*this, buffer);
	}

	Iterator iterator() {
		return Iterator();
	}

	size_t write(const Buffer& buffer) {
		AsioTask2 task;
		boost::asio::async_write(_handle, buffer.usefulData(), task.callback());
		return task.wait(_handle);
	}

	size_t read(Buffer* buffer) {
		AsioTask2 task;
		boost::asio::async_read(_handle, buffer->freeSpace(), task.callback());
		return task.wait(_handle);
	}

	size_t writeSome(const Buffer& buffer) {
		AsioTask2 task;
		_handle.async_write_some(buffer.usefulData(), task.callback());
		return task.wait(_handle);
	}

	size_t readSome(Buffer* buffer) {
		AsioTask2 task;
		_handle.async_read_some(buffer->freeSpace(), task.callback());
		return task.wait(_handle);
	}

protected:
	Handle _handle;
};