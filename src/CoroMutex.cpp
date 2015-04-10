
#include "CoroMutex.h"
#include "Coro.h"

void CoroMutex::lock() {
	{
		std::unique_lock<std::mutex> lock(_mutex);

		if (!_isLocked) {
			_isLocked = true;
			return;
		}

		_coroQueue.push(Coro::current());
	}

	Coro::current()->yield();
}

void CoroMutex::unlock() {
	std::unique_lock<std::mutex> lock(_mutex);

	if (_coroQueue.size()) {
		_coroQueue.front()->schedule();
		_coroQueue.pop();
	} else {
		_isLocked = false;
	}
}