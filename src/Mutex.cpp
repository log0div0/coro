
#include "coro/Mutex.h"
#include "coro/Finally.h"

void Mutex::lock() {
	if (_owner) {
		Finally cleanup([&] {
			_coros.remove(Coro::current());
		});
		_coros.push_back(Coro::current());
		_coros.back()->yield();
	}

	assert(_owner == nullptr);
	_owner = Coro::current();
}

void Mutex::unlock() {
	assert(_owner == Coro::current());
	_owner = nullptr;

	if (!_coros.empty()) {
		_coros.front()->resume();
	}
}
