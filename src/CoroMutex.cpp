
#include "CoroMutex.h"
#include "Coro.h"

void CoroMutex::lock() {
	auto task = std::make_shared<Task>();
	{
		std::unique_lock<std::mutex> lock(_mutex);

		if (!_isLocked) {
			_isLocked = true;
			return;
		}

		_taskQueue.push(task);
	}

	try {
		Coro::current()->yield();
	}
	catch (...) {
		task->cancel();
		unlock(); // следующий ...
		throw;
	}
}

void CoroMutex::unlock() {
	std::unique_lock<std::mutex> lock(_mutex);

	if (_taskQueue.size()) {
		(*_taskQueue.front())();
		_taskQueue.pop();
	} else {
		_isLocked = false;
	}
}