
#include "CoroMutex.h"
#include "ThreadPool.h"
#include "Coro.h"

void CoroMutex::lock() {
	std::unique_lock<std::mutex> lock(_mutex);

	if (!_isLocked) {
		_isLocked = true;
		return;
	}

	_taskQueue.push(Task::current());

	CoroYield([&]() {
		lock.unlock();
	});
}

void CoroMutex::unlock() {
	std::unique_lock<std::mutex> lock(_mutex);

	Task task;
	if (_taskQueue.size()) {
		task = std::move(_taskQueue.front());
		_taskQueue.pop();
	} else {
		_isLocked = false;
	}

	lock.unlock();

	task();
}