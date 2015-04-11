
#pragma once

#include <boost/asio/steady_timer.hpp>
#include "Coro.h"

class TimeoutError: public std::runtime_error {
public:
	TimeoutError(): std::runtime_error("TimeoutError") {}
};

class Timeout {
public:
	template <typename Duration>
	Timeout(Duration duration): _timer(ThreadPool::current()->ioService())
	{
		auto coro = Coro::current();

		_timer.expires_from_now(duration);

		auto callback = [=](const boost::system::error_code& errorCode) {
			if (errorCode == boost::asio::error::operation_aborted) {
				return;
			}
			coro->setException(TimeoutError());
			coro->resume();
		};

		_timer.async_wait(coro->strand()->wrap(callback));
	}

	~Timeout() {
		_timer.cancel();
	}

private:
	boost::asio::steady_timer _timer;
};