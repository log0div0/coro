
#include "CoroMutex.h"
#include "ThreadPool.h"
#include "Coro.h"

void CoroMutex::lock() {
	_mutex.lock();

	if (!_isLocked) {
		_isLocked = true;
		_mutex.unlock();
		return;
	}

	_coroQueue.push([threadPool = ThreadPool::current(), coro = Coro::current()] {
		threadPool->schedule([&](){
			coro->resume();
		});
	});

	Coro::current()->yield([&]() {
		_mutex.unlock();
	});
}

void CoroMutex::unlock() {
	std::lock_guard<std::mutex> lock(_mutex);

	if (_coroQueue.size()) {
		_coroQueue.front()();
		_coroQueue.pop();
	} else {
		_isLocked = false;
	}
}