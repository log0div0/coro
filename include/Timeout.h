
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
	Timeout(Duration duration): _timer(ThreadPool::current()->ioService()) {
		_timer.expires_from_now(duration);
		auto coro = Coro::current();
		_timer.async_wait([=](const boost::system::error_code& errorCode) {
			if (errorCode == boost::asio::error::operation_aborted) {
				return;
			}
			coro->executeSerially([=]() {
				coro->setException(TimeoutError());
				coro->schedule();
			});
		});
	}

	~Timeout() {
		_timer.cancel();
	}

private:
	boost::asio::steady_timer _timer;
};