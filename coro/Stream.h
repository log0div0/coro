
#pragma once


#include "coro/AsioTask.h"
#include "coro/Mutex.h"


template <typename Handle>
class Stream {
public:
	Stream(Handle handle): _handle(std::move(handle)) {}

	// VS2013 не умеет их генерить
	Stream(Stream&& other): _handle(std::move(other._handle)) {}

	Stream& operator=(Stream&& other) {
		_handle = std::move(other._handle);
		return *this;
	}

	template <typename T>
	size_t write(const T& t) {
		std::lock_guard<Mutex> lock(_writeMutex);
		AsioTask2<size_t> task;
		boost::asio::async_write(_handle, t, task.callback());
		return task.wait(_handle);
	}

	template <typename T>
	size_t read(const T& t) {
		std::lock_guard<Mutex> lock(_readMutex);
		AsioTask2<size_t> task;
		boost::asio::async_read(_handle, t, task.callback());
		return task.wait(_handle);
	}

	template <typename T>
	size_t writeSome(const T& t) {
		std::lock_guard<Mutex> lock(_writeMutex);
		AsioTask2<size_t> task;
		_handle.async_write_some(t, task.callback());
		return task.wait(_handle);
	}

	template <typename T>
	size_t readSome(const T& t) {
		std::lock_guard<Mutex> lock(_readMutex);
		AsioTask2<size_t> task;
		_handle.async_read_some(t, task.callback());
		return task.wait(_handle);
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