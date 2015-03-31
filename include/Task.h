
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

	Task(const Task& other): Task(other._threadPool, other._coro) {}

	Task& operator=(const Task& other) {
		_threadPool = other._threadPool;
		_coro = other._coro;
		return *this;
	}

	Task(Task&& other): Task(other) {
		other._threadPool = nullptr;
		other._coro = nullptr;
	}

	Task& operator=(Task&& other) {
		this->operator=(other);
		other._threadPool = nullptr;
		other._coro = nullptr;
		return *this;
	}

	void operator()() {
		if (_threadPool && _coro) {
			Task task = std::move(*this);
			task._threadPool->schedule([=]() {
				task._coro->resume();
			});
		}
	}

	template <typename Exception>
	void terminate(Exception exception) {
		if (_threadPool && _coro) {
			Task task = std::move(*this);
			task._threadPool->schedule([=]() {
				task._coro->resume(exception);
			});
		}
	}

private:
	ThreadPool* _threadPool;
	Coro* _coro;
};
