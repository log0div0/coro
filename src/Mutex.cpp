
#include "Mutex.h"

void Mutex::lock() {
	if (_owner) {
		try {
			_nonOwners.push_back(Coro::current());
			_nonOwners.back()->yield();
		}
		catch (...) {
			_nonOwners.remove(Coro::current());
			throw;
		}
	}

	assert(_owner == nullptr);
	_owner = Coro::current();
}

void Mutex::unlock() {
	assert(_owner == Coro::current());
	_owner = nullptr;

	if (!_nonOwners.empty()) {
		auto coro = _nonOwners.front();
		_nonOwners.pop_front();
		coro->resume();
	}
}
