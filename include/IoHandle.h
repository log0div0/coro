
#pragma once


#include "IoHandleIterator.h"
#include "Coro.h"


class IoTask: public std::enable_shared_from_this<IoTask> {
public:
	std::function<void(const boost::system::error_code&, size_t)> callback() {
		return coro->strand()->wrap(
			[task = shared_from_this()](const boost::system::error_code& errorCode, size_t bytesTranfered) {
				if (!task->coro) {
					return;
				}
				if (errorCode) {
					task->errorCode = errorCode;
				} else {
					task->bytesTranfered = bytesTranfered;
				}
				task->coro->resume();
			}
		);
	}

	template <typename Handle>
	size_t wait(Handle& handle) {
		try {
			coro->yield();
		}
		catch (...) {
			coro = nullptr;
			handle.cancel();
			throw;
		}

		if (errorCode) {
			throw boost::system::system_error(errorCode);
		}

		return bytesTranfered;
	}

private:
	Coro* coro = Coro::current();
	boost::system::error_code errorCode;
	size_t bytesTranfered = 0;
};


template <typename Handle>
class IoHandle {
public:
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
		auto task = std::make_shared<IoTask>();
		boost::asio::async_write(_handle, buffer.usefulData(), task->callback());
		return task->wait(_handle);
	}

	/**
	  * @brief Принять хоть сколько-нибудь байт в buffer->freeSpace()
	  * @return Кол-во принятых байт
	  */
	size_t readSome(Buffer* buffer) {
		auto task = std::make_shared<IoTask>();
		_handle.async_read_some(buffer->freeSpace(), task->callback());
		return task->wait(_handle);
	}

protected:
	Handle _handle;
};