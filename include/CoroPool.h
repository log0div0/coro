
#pragma once

#include "Coro.h"
#include <set>

class CoroPool {
public:
	CoroPool();
	~CoroPool();
	Coro* exec(std::function<void()> routine);
	void join();

private:
	void onCoroDone(Coro* coro);

	std::set<Coro*> _execCoros, _joinCoros;
};
