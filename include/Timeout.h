
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
	Timeout(Duration duration)
		: _timer(ThreadPool::current()->ioService()),
		  _coro(new Coro*(Coro::current()))
	{
		_timer.expires_from_now(duration);

		auto callback = [coro = _coro](const boost::system::error_code& errorCode) {
			if (errorCode == boost::asio::error::operation_aborted) {
				return;
			}
			if (*coro) {
				(*coro)->setException(TimeoutError());
				(*coro)->resume();
			}
		};

		_timer.async_wait((*_coro)->strand()->wrap(callback));
	}

	~Timeout() {
		*_coro = nullptr;
		_timer.cancel();
	}

private:
	boost::asio::steady_timer _timer;
	std::shared_ptr<Coro*> _coro;
};