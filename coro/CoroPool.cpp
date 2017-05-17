
#include "coro/Strand.h"
#include "coro/CoroPool.h"
#include <coro/Finally.h>

namespace coro {

CoroPool::~CoroPool() {
	cancelAll();
	waitAll(true);
}

CoroPool::CoroPool(CoroPool&& other):
	_parentCoro(std::move(other._parentCoro)),
	_childCoros(std::move(other._childCoros))
{
	other._parentCoro = nullptr;
}

CoroPool& CoroPool::operator=(CoroPool&& other) {
	std::swap(_parentCoro, other._parentCoro);
	std::swap(_childCoros, other._childCoros);
	return *this;
}

void CoroPool::assertCurrentIsParent() {
	if (Coro::current() != _parentCoro) {
		throw std::runtime_error(
			"Пулом корутин можно пользоватся только в той корутине, "
			"в которой этот пул был создан. Реорганизуйте Ваш код так, "
			"чтобы пулы корутин образовывали строгую иерархию."
		);
	}
}

Coro* CoroPool::exec(std::function<void()> routine) {
	assertCurrentIsParent();

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
	assertCurrentIsParent();

	while(!_childCoros.empty()) {
		if (noThrow) {
			_parentCoro->yield({token()});
		} else {
			_parentCoro->yield({token(), TokenThrow});
		}
	}
}

void CoroPool::cancelAll() {
	assertCurrentIsParent();

	for (auto coro: _childCoros) {
		if (_childCoros.find(coro) != _childCoros.end()) {
			coro->propagateException(CancelError());
		}
	}
}

void CoroPool::onCoroDone(Coro* childCoro) {
	Strand::current()->post([=] {
		_childCoros.erase(childCoro);
		while (childCoro->exceptions().size()) {
			try {
				childCoro->propagateException();
			}
			catch (const CancelError&) {
				// CancelError не пробрасываем в родительскую корутину
				// (пытаться отменить корутину можно сколько угодно раз, поэтому исключений может быть несколько)
			}
			catch (...) {
				_parentCoro->propagateException(std::current_exception());
			}
		}
		delete childCoro;

		_parentCoro->resume(token());
	});
}

std::string CoroPool::token() const {
	return "CoroPool " + std::to_string((uint64_t)this);
}

size_t CoroPool::size() const {
	return _childCoros.size();
}

void WaitOne(std::initializer_list<std::function<void()>> routines) {
	CoroPool pool;
	for (auto& routine: routines) {
		pool.exec(std::move(routine));
	}
	Coro::current()->yield({pool.token(), TokenThrow});
}

}
