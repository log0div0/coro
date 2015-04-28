
#include "CoroPool.h"
#include "IoService.h"

CoroPool::CoroPool(bool killOnJoin): _killOnJoin(killOnJoin) {
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

	_joinCoros.insert(Coro::current());

	try {
		Coro::current()->yield();
	}
	catch (...) {
		_joinCoros.erase(Coro::current());
		throw;
	}
}

void CoroPool::onCoroDone(Coro* coro) {
	_execCoros.erase(coro);
	delete coro;

	if (_execCoros.empty()) {
		auto joinCoros = std::move(_joinCoros);
		for (auto coro: joinCoros) {
			coro->resume();
		}
	}
}
