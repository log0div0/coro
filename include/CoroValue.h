
#pragma once

#include "ThreadPool.h"
#include "Coro.h"
#include <mutex>

class Task {
public:
	Task(ThreadPool* threadPool = nullptr, Coro* coro = nullptr)
		: _threadPool(threadPool),
		  _coro(coro)
	{

	}

	static Task current() {
		return Task(ThreadPool::current(), Coro::current());
	}

	Task(Task&& other)
		: _threadPool(other._threadPool),
		  _coro(other._coro)
	{
		other._threadPool = nullptr;
		other._coro = nullptr;
	}

	Task& operator=(Task&& other) {
		_threadPool = other._threadPool;
		_coro = other._coro;
		other._threadPool = nullptr;
		other._coro = nullptr;
		return *this;
	}

	void operator()() {
		if (_threadPool && _coro) {
			ThreadPool* threadPool = _threadPool;
			Coro* coro = _coro;
			_threadPool = nullptr;
			_coro = nullptr;
			threadPool->schedule([=]() {
				coro->resume();
			});
		}
	}

private:
	ThreadPool* _threadPool;
	Coro* _coro;
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
		wait();
		return _t;
	}

	T* operator->() {
		wait();
		return &_t;
	}

	void wait() {
		std::unique_lock<std::mutex> lock(_mutex);

		if (_isSet) {
			return;
		}

		_consumer = Task::current();

		Coro::current()->yield([&]() {
			lock.unlock();
		});
	}

private:
	std::mutex _mutex;
	T _t;
	bool _isSet;
	Task _producer, _consumer;
};