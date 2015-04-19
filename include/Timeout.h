
#pragma once

#include <boost/asio/steady_timer.hpp>
#include "Coro.h"

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
	class Task {
	public:
		Task(uint64_t id): _coro(Coro::current()), _id(id) {}

		void execute() {
			if (_coro) {
				_coro->setException(TimeoutError(_id));
				_coro->resume();
			}
		}
		void preventExecution() {
			_coro = nullptr;
		}

	private:
		Coro* _coro;
		uint64_t _id;
	};

	template <typename Duration>
	Timeout(Duration duration)
		: _timer(ThreadPool::current()->ioService()),
		  _id(++idCounter),
		  _task(new Task(_id))
	{
		_timer.expires_from_now(duration);

		auto callback = [task = _task](const boost::system::error_code& errorCode) {
			if (errorCode == boost::asio::error::operation_aborted) {
				return;
			}
			task->execute();
		};

		_timer.async_wait(Coro::current()->strand()->wrap(callback));
	}

	~Timeout() {
		_task->preventExecution();
		_timer.cancel();
	}

	uint64_t id() const {
		return _id;
	}

private:
	boost::asio::steady_timer _timer;
	static std::atomic<uint64_t> idCounter;
	uint64_t _id;
	std::shared_ptr<Task> _task;
};