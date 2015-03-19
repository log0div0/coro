
#pragma once

#include "ThreadPool.h"
#include "Coro.h"

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

	template <typename Exception>
	void terminate(Exception exception) {
		if (_threadPool && _coro) {
			ThreadPool* threadPool = _threadPool;
			Coro* coro = _coro;
			_threadPool = nullptr;
			_coro = nullptr;
			threadPool->schedule([=]() {
				coro->resume(exception);
			});
		}
	}

private:
	ThreadPool* _threadPool;
	Coro* _coro;
};
