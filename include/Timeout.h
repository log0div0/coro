
#pragma once

#include <boost/asio/steady_timer.hpp>
#include "Coro.h"
#include "IoService.h"

// Без decltype конструкция наподобии
// Timeout timeout(std::chrono::seconds(3))
// НЕ ПОПАДАЕТ в бинарник
// WTF????
#define DECLARE_TIMEOUT(duration) Timeout timeout((decltype(duration))duration)

class TimeoutError: public std::runtime_error {
public:
	TimeoutError(uint64_t timeoutId): std::runtime_error("TimeoutError"), _timeoutId(timeoutId) {}

	uint64_t timeoutId() const {
		return _timeoutId;
	}

private:
	uint64_t _timeoutId;
};

class Timeout {
public:
	template <typename Duration>
	Timeout(Duration duration): _timer(*IoService::current()) {
		_timer.expires_from_now(duration);
		_timer.async_wait([=](const boost::system::error_code& errorCode) {
			_callbackExecuted = true;
			if (errorCode == boost::asio::error::operation_aborted) {
				return _coro->resume();
			}
			if (errorCode) {
				return _coro->resume(boost::system::system_error(errorCode));
			}
			_coro->resume(TimeoutError(_id));
		});
	}

	~Timeout() {
		_timer.cancel();
		waitCallbackExecution();
	}

	uint64_t id() const {
		return _id;
	}

private:
	void waitCallbackExecution() {
		std::vector<std::exception_ptr> exceptions;
		while (!_callbackExecuted) {
			try {
				_coro->yield();
			}
			catch (...) {
				exceptions.push_back(std::current_exception());
			}
		}
		for (auto exception: exceptions) {
			_coro->throwOnResumeOrYield(exception);
		}
	}

	boost::asio::steady_timer _timer;
	static std::atomic<uint64_t> idCounter;
	uint64_t _id = ++idCounter;
	bool _callbackExecuted = false;
	Coro* _coro = Coro::current();
};