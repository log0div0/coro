
#include "CoroMutex.h"
#include "Coro.h"

void CoroMutex::lock() {
	auto task = std::make_shared<CoroTask>();
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
		std::unique_lock<std::mutex> lock(_mutex);
		task->preventExecution();
		next();
		throw;
	}
}

void CoroMutex::unlock() {
	std::unique_lock<std::mutex> lock(_mutex);
	next();
}

void CoroMutex::next() {
	while (_taskQueue.size()) {
		auto task = std::move(_taskQueue.front());
		_taskQueue.pop();
		if (task->schedule()) {
			return;
		}
	}
	_isLocked = false;
}