
#include "CoroMutex.h"
#include "ThreadPool.h"
#include "Coro.h"

void CoroMutex::lock() {
	auto lock = std::make_shared<
		std::lock_guard<std::mutex>
	>(_mutex);

	if (!_isLocked) {
		_isLocked = true;
		return;
	}

	_coroQueue.push([threadPool = ThreadPool::current(), coro = Coro::current()] {
		threadPool->schedule([=](){
			coro->resume();
		});
	});

	Coro::current()->yield([lock = std::move(lock)]() {

	});
}

void CoroMutex::unlock() {
	std::function<void()> callback;

	{
		std::lock_guard<std::mutex> lock(_mutex);

		if (_coroQueue.size()) {
			callback = std::move(_coroQueue.front());
			_coroQueue.pop();
		} else {
			_isLocked = false;
		}
	}

	if (callback) {
		callback();
	}
}