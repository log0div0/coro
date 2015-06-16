
#pragma once

#include "coro/Coro.h"
#include <set>

class CoroPool {
public:
	CoroPool(bool killOnJoin = false);
	~CoroPool();
	Coro* exec(std::function<void()> routine);
	void join();

private:
	void onCoroDone(Coro* coro);

	std::set<Coro*> _execCoros;
	Coro* _rootCoro = Coro::current();
	bool _killOnJoin, _joinCalled;
};
