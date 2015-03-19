
#pragma once

#include "Task.h"
#include <mutex>


class InvalidValueError: public std::runtime_error {
public:
	InvalidValueError(): std::runtime_error("InvalidValueError") {}
};


template <typename T>
class CoroValue {
public:
	CoroValue()
		: _isSet(false)
	{

	}

	CoroValue(T t)
		: _t(std::forward<T>(t)),
		  _isSet(true)
	{

	}

	CoroValue(CoroValue&& other) = delete;
	CoroValue& operator=(CoroValue&& other) = delete;

	void set(T t) {
		std::unique_lock<std::mutex> lock(_mutex);

		_t = std::forward<T>(t);
		_isSet = true;

		_producer = Task::current();
		Coro::current()->yield([&]() {
			lock.unlock();
			_consumer();
		});
	}

	void reset() {
		_isSet = false;
		_producer();
	}

	T& operator*() {
		return _t;
	}

	T* operator->() {
		return &_t;
	}

	void wait() {
		std::unique_lock<std::mutex> lock(_mutex);

		if (!_isValid) {
			throw InvalidValueError();
		}

		if (_isSet) {
			return;
		}

		_consumer = Task::current();
		Coro::current()->yield([&]() {
			lock.unlock();
		});
	}

	void invalidate() {
		std::unique_lock<std::mutex> lock(_mutex);
		_isValid = false;
		_consumer.terminate(InvalidValueError());
	}

	void lock() {
		wait();
	}

	void unlock() {
		reset();
	}

private:
	std::mutex _mutex;
	T _t;
	bool _isValid = true;
	bool _isSet;
	Task _producer, _consumer;
};