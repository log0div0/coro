
#pragma once


#include "coro/StreamIterator.h"
#include "coro/AsioTask.h"
#include "coro/Mutex.h"


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

	template <typename T>
	size_t doWrite(const T& t) {
		std::lock_guard<Mutex> lock(_writeMutex);
		AsioTask2<size_t> task;
		boost::asio::async_write(_handle, t, task.callback());
		return task.wait(_handle);
	}

	template <typename T>
	size_t doRead(const T& t) {
		std::lock_guard<Mutex> lock(_readMutex);
		AsioTask2<size_t> task;
		boost::asio::async_read(_handle, t, task.callback());
		return task.wait(_handle);
	}

	size_t write(const Buffer& buffer) {
		return doWrite(buffer.usefulData());
	}

	size_t read(Buffer* buffer) {
		return doRead(buffer->freeSpace());
	}

	template <typename T>
	size_t doWriteSome(const T& t) {
		std::lock_guard<Mutex> lock(_writeMutex);
		AsioTask2<size_t> task;
		_handle.async_write_some(t, task.callback());
		return task.wait(_handle);
	}

	template <typename T>
	size_t doReadSome(const T& t) {
		std::lock_guard<Mutex> lock(_readMutex);
		AsioTask2<size_t> task;
		_handle.async_read_some(t, task.callback());
		return task.wait(_handle);
	}

	size_t writeSome(const Buffer& buffer) {
		return doWriteSome(buffer.usefulData());
	}

	size_t readSome(Buffer* buffer) {
		return doReadSome(buffer->freeSpace());
	}

	Handle& handle() {
		return _handle;
	}

	const Handle& handle() const {
		return _handle;
	}

protected:
	Handle _handle;
	Mutex _readMutex, _writeMutex;
};