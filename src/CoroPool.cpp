
#include "coro/CoroPool.h"
#include "coro/IoService.h"
#include "coro/Finally.h"

CoroPool::CoroPool(bool killOnJoin): _killOnJoin(killOnJoin), _joinCalled(false) {
}

CoroPool::~CoroPool() {
	join();
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

void CoroPool::join() {
	if (_execCoros.empty()) {
		return;
	}

	if (_killOnJoin) {
		for (auto coro: _execCoros) {
			IoService::current()->post([=] {
				coro->resume(CancelError());
			});
		}
	}

	_joinCalled = true;
	_rootCoro->yieldNoThrow();
	_joinCalled = false;

	assert(_execCoros.empty());
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

		if (_execCoros.empty() && _joinCalled) {
			_rootCoro->resume();
		}
	});
}
