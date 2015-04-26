
#pragma once


#include "Coro.h"
#include <boost/asio.hpp>


class AsioTask1 {
public:
	std::function<void(const boost::system::error_code&)> callback() {
		return [=](const boost::system::error_code& errorCode) {
			_errorCode = errorCode;
			_coro->resume();
		};
	}

	template <typename Handle>
	void wait(Handle& handle) {
		try {
			_coro->yield();
		}
		catch (...) {
			handle.cancel();
			try {
				_coro->yield();
			}
			catch (...) {
				assert(false);
			}
			throw;
		}

		if (_errorCode) {
			throw boost::system::system_error(_errorCode);
		}
	}

private:
	Coro* _coro = Coro::current();
	boost::system::error_code _errorCode;
};


class AsioTask2 {
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
		try {
			_coro->yield();
		}
		catch (...) {
			handle.cancel();
			try {
				_coro->yield();
			}
			catch (...) {
				assert(false);
			}
			throw;
		}

		if (_errorCode) {
			throw boost::system::system_error(_errorCode);
		}

		return _bytesTranfered;
	}

private:
	Coro* _coro = Coro::current();
	boost::system::error_code _errorCode;
	size_t _bytesTranfered = 0;
};
