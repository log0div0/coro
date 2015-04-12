
#pragma once

#include "IoHandleIterator.h"
#include "Coro.h"


template <typename Handle>
class IoHandle {
public:
	struct Task {
		Coro* coro;
		boost::system::error_code errorCode;
		size_t bytesTranfered = 0;
	};

	typedef IoHandleIterator<IoHandle> Iterator;

	IoHandle(Handle handle): _handle(std::move(handle)) {}

	// VS2013 не умеет их генерить
	IoHandle(IoHandle&& other): _handle(std::move(other._handle)) {}

	IoHandle& operator=(IoHandle&& other) {
		_handle = std::move(other._handle);
		return *this;
	}

	Iterator iterator(Buffer& buffer) {
		return Iterator(*this, buffer);
	}

	Iterator iterator() {
		return Iterator();
	}

	/**
	  * @brief Отправить buffer.usefulData() целиком
	  * @return Кол-во отправленных байт
	  */
	size_t write(const Buffer& buffer) {
		auto coro = Coro::current();

		auto task = std::make_shared<Task>();
		task->coro = coro;

		auto callback = [task](const boost::system::error_code& errorCode, size_t bytesTranfered) {
			if (!task->coro) {
				return;
			}
			if (errorCode) {
				task->errorCode = errorCode;
			} else {
				task->bytesTranfered = bytesTranfered;
			}
			task->coro->resume();
		};

		boost::asio::async_write(_handle, buffer.usefulData(), coro->strand()->wrap(callback));

		try {
			task->coro->yield();
		}
		catch (...) {
			task->coro = nullptr;
			_handle.cancel();
			throw;
		}

		if (task->errorCode) {
			throw boost::system::system_error(task->errorCode);
		}

		return task->bytesTranfered;
	}

	/**
	  * @brief Принять хоть сколько-нибудь байт в buffer->freeSpace()
	  * @return Кол-во принятых байт
	  */
	size_t read(Buffer* buffer) {
		auto coro = Coro::current();

		auto task = std::make_shared<Task>();
		task->coro = coro;

		auto callback = [task](const boost::system::error_code& errorCode, size_t bytesTranfered) {
			if (!task->coro) {
				return;
			}
			if (errorCode) {
				task->errorCode = errorCode;
			} else {
				task->bytesTranfered = bytesTranfered;
			}
			task->coro->resume();
		};

		_handle.async_read_some(buffer->freeSpace(), coro->strand()->wrap(callback));

		try {
			task->coro->yield();
		}
		catch (...) {
			task->coro = nullptr;
			_handle.cancel();
			throw;
		}

		if (task->errorCode) {
			throw boost::system::system_error(task->errorCode);
		}

		return task->bytesTranfered;
	}

protected:
	Handle _handle;
};