
#pragma once


#include "coro/Coro.h"
#include <boost/asio.hpp>


class AsioTask {
protected:
	template <typename Handle>
	void doWait(Handle& handle) {
		std::exception_ptr exception;
		try {
			_coro->yield();
		}
		catch (...) {
			exception = std::current_exception();
		}
		// не вызывайте yield/resume внутри catch
		// ограничение boost::context
		if (exception) {
			handle.cancel();
			_coro->yieldNoThrow();
			assert(_isCallbackExecuted);
			std::rethrow_exception(exception);
		}
	}

	Coro* _coro = Coro::current();
	bool _isCallbackExecuted = false;
};


class AsioTask1: public AsioTask {
public:
	std::function<void(const boost::system::error_code&)> callback() {
		return [=](const boost::system::error_code& errorCode) {
			_isCallbackExecuted = true;
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


template <typename Result>
class AsioTask2: public AsioTask {
public:
	std::function<void(const boost::system::error_code&, Result)> callback() {
		return [=](const boost::system::error_code& errorCode, Result result) {
			_isCallbackExecuted = true;
			_errorCode = errorCode;
			_result = result;
			_coro->resume();
		};
	}

	template <typename Handle>
	Result wait(Handle& handle) {
		doWait(handle);

		if (_errorCode) {
			throw boost::system::system_error(_errorCode);
		}

		return _result;
	}

private:
	boost::system::error_code _errorCode;
	Result _result;
};
