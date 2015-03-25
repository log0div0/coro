
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
		_consumer();
	}

	void reset() {
		std::unique_lock<std::mutex> lock(_mutex);
		_isSet = false;
		_producer();
	}

	void wait_for_set() {
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

	void wait_for_reset() {
		std::unique_lock<std::mutex> lock(_mutex);

		if (!_isValid) {
			throw InvalidValueError();
		}

		if (!_isSet) {
			return;
		}

		_producer = Task::current();
		Coro::current()->yield([&]() {
			lock.unlock();
		});
	}

	T& operator*() {
		return _t;
	}

	T* operator->() {
		return &_t;
	}

	void invalidate() {
		std::unique_lock<std::mutex> lock(_mutex);
		_isValid = false;
		_producer.terminate(InvalidValueError());
		_consumer.terminate(InvalidValueError());
	}

	void lock() {
		wait_for_set();
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