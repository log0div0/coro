
#include "CoroPool.h"
#include "IoService.h"

CoroPool::CoroPool(bool killOnJoin): _killOnJoin(killOnJoin), _joinCalled(false) {
}

CoroPool::~CoroPool() {
	join();
}

class GC {
public:
	GC(std::function<void()> fn): _fn(std::move(fn)) {}
	~GC() {
		IoService::current()->post(_fn);
	}

private:
	std::function<void()> _fn;
};

Coro* CoroPool::exec(std::function<void()> routine) {
	auto coro = new Coro([=] {
		GC gc(std::bind(&CoroPool::onCoroDone, this, Coro::current()));
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
			coro->resume(CancelError());
		}
	}

	_joinCalled = true;

	std::exception_ptr exception;
	try {
		_rootCoro->yield();
	}
	catch (...) {
		exception = std::current_exception();
	}
	// не вызывайте yield/resume внутри catch
	// ограничение boost::context
	if (exception) {
		_rootCoro->yieldNoThrow();
	}

	_joinCalled = false;

	assert(_execCoros.empty());

	if (exception) {
		std::rethrow_exception(exception);
	}
}

void CoroPool::onCoroDone(Coro* coro) {
	_execCoros.erase(coro);
	for (auto exception: coro->exceptions()) {
		assert(exception);
		_rootCoro->resume(exception);
	}
	delete coro;

	if (_execCoros.empty() && _joinCalled) {
		_rootCoro->resume();
	}
}
