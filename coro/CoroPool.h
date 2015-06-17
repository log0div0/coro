
#pragma once

#include "coro/Coro.h"
#include <set>

class CoroPool {
public:
	CoroPool();
	~CoroPool();
	Coro* exec(std::function<void()> routine);
	void waitAll();
	void killAll();

private:
	void onCoroDone(Coro* coro);

	std::set<Coro*> _execCoros;
	Coro* _rootCoro = Coro::current();
	bool _wakeUpRoot;
};
