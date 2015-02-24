
#include "CoroMutex.h"
#include "ThreadPool.h"
#include "Coro.h"

void CoroMutex::lock() {
	{
		std::lock_guard<std::mutex> lock(_mutex);

		if (!_isLocked) {
			_isLocked = true;
			return;
		}

		_queue.push([threadPool = ThreadPool::current(), coro = Coro::current()] {
			threadPool->schedule([&](){
				coro->resume();
			});
		});
	}

	Coro::current()->yield();
}

void CoroMutex::unlock() {
	std::lock_guard<std::mutex> lock(_mutex);

	if (_queue.size()) {
		_queue.front()();
		_queue.pop();
	} else {
		_isLocked = false;
	}
}