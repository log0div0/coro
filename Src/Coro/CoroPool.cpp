
#include "Coro/Strand.h"
#include "Coro/CoroPool.h"
#include <Utils/Finally.h>

namespace coro {

CoroPool::~CoroPool() {
	cancelAll();
	waitAll(true);
}

Coro* CoroPool::exec(std::function<void()> routine) {
	auto coro = new Coro([=] {
		Finally cleanup([=] {
			onCoroDone(Coro::current());
		});

		routine();
	});
	_childCoros.insert(coro);
	coro->start();
	return coro;
}

void CoroPool::waitAll(bool noThrow) {
	if (_childCoros.empty()) {
		return;
	}

	if (noThrow) {
		_parentCoro->yield({token()});
	} else {
		_parentCoro->yield({token(), TokenThrow});
	}

	assert(_childCoros.empty());
}

void CoroPool::cancelAll() {
	for (auto coro: _childCoros) {
		if (_childCoros.find(coro) != _childCoros.end()) {
			coro->throwException(CancelError());
		}
	}
}

void CoroPool::onCoroDone(Coro* childCoro) {
	Strand::current()->post([=] {
		_childCoros.erase(childCoro);
		for (auto exception: childCoro->exceptions()) {
			assert(exception);
			try {
				std::rethrow_exception(exception);
			}
			catch (const CancelError&) {
				// CancelError не пробрасываем в родительскую корутину
				// (пытаться отменить корутину можно сколько угодно раз, поэтому исключений может быть несколько)
			}
			catch (...) {
				_parentCoro->throwException(std::current_exception());
			}
		}
		delete childCoro;

		if (_childCoros.empty()) {
			_parentCoro->resume(token());
		}
	});
}

std::string CoroPool::token() const {
	return "CoroPool " + std::to_string((uint64_t)this);
}

}