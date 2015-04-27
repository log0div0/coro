
#pragma once


#include "Coro.h"
#include <boost/asio.hpp>


class AsioTask {
protected:
	template <typename Handle>
	void doWait(Handle& handle) {
		try {
			_coro->yield();
		}
		catch (...) {
			handle.cancel();
			waitCallbackExecution();
			throw;
		}
	}

	void waitCallbackExecution() {
		std::vector<std::exception_ptr> exceptions;
		while (true) {
			try {
				_coro->yield();
				break;
			}
			catch (...) {
				exceptions.push_back(std::current_exception());
			}
		};
		for (auto exception: exceptions) {
			_coro->throwOnResumeOrYield(exception);
		}
	}

	Coro* _coro = Coro::current();
};


class AsioTask1: public AsioTask {
public:
	std::function<void(const boost::system::error_code&)> callback() {
		return [=](const boost::system::error_code& errorCode) {
			_errorCode = errorCode;
			_coro->resume();
		};
	}

	template <typename Handle>
	void wait(Handle& handle) {
		doWait(handle);

		if (_errorCode) {
			throw boost::system::system_error(_errorCode);
		}
	}

private:
	boost::system::error_code _errorCode;
};


class AsioTask2: public AsioTask {
public:
	std::function<void(const boost::system::error_code&, size_t)> callback() {
		return [=](const boost::system::error_code& errorCode, size_t bytesTranfered) {
			_errorCode = errorCode;
			_bytesTranfered = bytesTranfered;
			_coro->resume();
		};
	}

	template <typename Handle>
	size_t wait(Handle& handle) {
		doWait(handle);

		if (_errorCode) {
			throw boost::system::system_error(_errorCode);
		}

		return _bytesTranfered;
	}

private:
	boost::system::error_code _errorCode;
	size_t _bytesTranfered = 0;
};
