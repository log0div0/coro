
#pragma once

#include "IoHandleIterator.h"
#include "Coro.h"

template <typename Handle>
class IoHandle {
public:
	typedef IoHandleIterator<IoHandle> Iterator;

	IoHandle(Handle handle): _handle(std::move(handle)) {}

	// VS2013 не умеет их генерить
	IoHandle(IoHandle&& other): _handle(std::move(other._handle)) {}

	IoHandle& operator=(IoHandle&& other) {
		_handle = std::move(other._handle);
	}

	Iterator iterator(Buffer& buffer) {
		return Iterator(*this, buffer);
	}

	Iterator iterator() {
		return Iterator();
	}

	/**
	  * @brief Отправить buffer.usefulData()
	  * @return Кол-во отправленных байт
	  */
	size_t sendData(const Buffer& buffer) {
		Coro& coro = *Coro::current();
		boost::system::error_code errorCode;
		size_t bytesTranfered = 0;

		coro.yield([&]() {
			boost::asio::async_write(_handle, buffer.usefulData(),
				[&](const boost::system::error_code& errorCode_, size_t bytesTranfered_) {
					if (errorCode_) {
						errorCode = errorCode_;
					} else {
						bytesTranfered = bytesTranfered_;
					}
					coro.resume();
				}
			);
		});

		if (errorCode) {
			throw boost::system::system_error(errorCode);
		}

		return bytesTranfered;
	}

	/**
	  * @brief Принять хоть сколько-нибудь байт (и записать их в конец буфера)
	  */
	void receiveData(Buffer* buffer) {
		Coro& coro = *Coro::current();
		boost::system::error_code errorCode;

		coro.yield([&]() {
			_handle.async_read_some(buffer->freeSpace(),
				[&](const boost::system::error_code& errorCode_, size_t bytesTranfered) {
					if (errorCode_) {
						errorCode = errorCode_;
					} else {
						buffer->pushBack(bytesTranfered);
					}
					coro.resume();
				}
			);
		});

		if (errorCode) {
			throw boost::system::system_error(errorCode);
		}
	}

protected:
	Handle _handle;
};