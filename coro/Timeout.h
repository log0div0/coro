
#pragma once

#include <boost/asio/steady_timer.hpp>
#include <atomic>
#include "coro/Coro.h"
#include "coro/IoService.h"

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
			if (_timerCanceled) {
				return _coro->resume();
			}
			// здесь никогда не окажемся, т.к. есть _timerCanceled
			// if (errorCode == boost::asio::error::operation_aborted) {
			// 	return _coro->resume();
			// }
			if (errorCode) {
				return _coro->resume(boost::system::system_error(errorCode));
			}
			_coro->resume(TimeoutError(_id));
		});
	}

	~Timeout() {
		if (!_callbackExecuted) {
			_timerCanceled = true;
			_timer.cancel();
			waitCallbackExecution();
		}
	}

	uint64_t id() const {
		return _id;
	}

private:
	void waitCallbackExecution() {
		_coro->yieldNoThrow();
	}

	boost::asio::steady_timer _timer;

	static std::atomic<uint64_t> idCounter;
	uint64_t _id = ++idCounter;

	Coro* _coro = Coro::current();
	bool _timerCanceled = false, _callbackExecuted = false;
};