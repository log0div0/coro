
#include "CoroMutex.h"
#include "Coro.h"

void CoroMutex::lock() {
	auto coro = std::make_shared<Coro*>(Coro::current());
	{
		std::unique_lock<std::mutex> lock(_mutex);

		if (!_isLocked) {
			_isLocked = true;
			return;
		}

		_coroQueue.push(coro);
	}

	try {
		Coro::current()->yield();
	}
	catch (...) {
		std::unique_lock<std::mutex> lock(_mutex);
		*coro = nullptr;
		next();
		throw;
	}
}

void CoroMutex::unlock() {
	std::unique_lock<std::mutex> lock(_mutex);
	next();
}

void CoroMutex::next() {
	while (_coroQueue.size()) {
		auto coro = std::move(_coroQueue.front());
		_coroQueue.pop();
		if (*coro == nullptr) {
			continue;
		}
		(*coro)->strand()->post([coro] {
			if (*coro) {
				(*coro)->resume();
			}
		});
		return;
	}
	_isLocked = false;
}