
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

class TimeoutTask: public std::enable_shared_from_this<TimeoutTask> {
public:
	TimeoutTask(uint64_t id): _coro(Coro::current()), _id(id) {}

	void preventExecution() {
		_coro = nullptr;
	}

	std::function<void(const boost::system::error_code&)> callback() {
		return [self = shared_from_this(), this](const boost::system::error_code&) {
			if (_coro) {
				_coro->resume(TimeoutError(_id));
			}
		};
	}
private:
	Coro* _coro;
	uint64_t _id;
};

class Timeout {
public:
	template <typename Duration>
	Timeout(Duration duration)
		: _timer(*IoService::current()),
		  _id(++idCounter),
		  _task(new TimeoutTask(_id))
	{
		_timer.expires_from_now(duration);
		_timer.async_wait(_task->callback());
	}

	~Timeout() {
		_task->preventExecution();
	}

	uint64_t id() const {
		return _id;
	}

private:
	boost::asio::steady_timer _timer;
	static std::atomic<uint64_t> idCounter;
	uint64_t _id;
	std::shared_ptr<TimeoutTask> _task;
};