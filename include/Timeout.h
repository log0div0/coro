
#pragma once

#include <boost/asio/steady_timer.hpp>
#include "Task.h"

class TimeoutError: public std::runtime_error {
public:
	TimeoutError(): std::runtime_error("TimeoutError") {}
};

class Timeout {
public:
	template <typename Duration>
	Timeout(Duration duration): _timer(ThreadPool::current()->ioService()), _coro(Coro::current())
	{
		_timer.expires_from_now(duration);

		auto callback = [&](const boost::system::error_code& errorCode) {
			if (errorCode == boost::asio::error::operation_aborted) {
				return;
			}
			if (_coro) {
				_coro->setException(TimeoutError());
				_coro->resume();
			}
		};

		_timer.async_wait(_coro->strand()->wrap(callback));
	}

	~Timeout() {
		_coro = nullptr;
		_timer.cancel();
	}

private:
	boost::asio::steady_timer _timer;
	Coro* _coro;
};