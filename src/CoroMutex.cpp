
#include "CoroMutex.h"
#include "ThreadPool.h"
#include "Coro.h"

void CoroMutex::lock() {
	std::unique_lock<std::mutex> lock(_mutex);

	if (!_isLocked) {
		_isLocked = true;
		return;
	}

	_coroQueue.push([threadPool = ThreadPool::current(), coro = Coro::current()] {
		threadPool->schedule([=](){
			coro->resume();
		});
	});

	Coro::current()->yield([&]() {
		lock.unlock();
	});
}

void CoroMutex::unlock() {
	std::unique_lock<std::mutex> lock(_mutex);

	std::function<void()> callback;
	if (_coroQueue.size()) {
		callback = std::move(_coroQueue.front());
		_coroQueue.pop();
	} else {
		_isLocked = false;
	}

	lock.unlock();

	if (callback) {
		callback();
	}
}