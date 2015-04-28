
#pragma once

#include "Coro.h"
#include <set>

class CoroPool {
public:
	CoroPool(bool killOnJoin = false);
	~CoroPool();
	Coro* exec(std::function<void()> routine);
	void join();

private:
	void onCoroDone(Coro* coro);

	std::set<Coro*> _execCoros, _joinCoros;
	bool _killOnJoin;
};
