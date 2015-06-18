
#include "coro/CoroPool.h"
#include "coro/IoService.h"
#include "coro/Finally.h"

CoroPool::CoroPool(): _wakeUpRoot(false) {
}

CoroPool::~CoroPool() {
	killAll();
}

Coro* CoroPool::exec(std::function<void()> routine) {
	auto coro = new Coro([=] {
		Finally cleanup([=] {
			onCoroDone(Coro::current());
		});

		routine();
	});
	_execCoros.insert(coro);

	coro->resume();

	return coro;
}

void CoroPool::waitAll() {
	if (_execCoros.empty()) {
		return;
	}

	_wakeUpRoot = true;
	_rootCoro->yieldNoThrow();
	_wakeUpRoot = false;

	assert(_execCoros.empty());
}

void CoroPool::killAll() {
	for (auto coro: _execCoros) {
		IoService::current()->post([=] {
			coro->resume(CancelError());
		});
	}

	waitAll();
}

void CoroPool::onCoroDone(Coro* coro) {
	IoService::current()->post([=] {
		_execCoros.erase(coro);
		for (auto exception: coro->_exceptions) {
			assert(exception);
			_rootCoro->resume(exception);
		}
		coro->_exceptions.clear();
		delete coro;

		if (_execCoros.empty() && _wakeUpRoot) {
			_rootCoro->resume();
		}
	});
}
