
#pragma once

#include "Coro.h"
#include <mutex>
#include <set>

class CoroPool {
public:
	void fork(std::function<void()> routine);
	void join();

private:
	std::mutex _mutex;
	std::set<Coro> _coros;
	std::function<void()> _onJoin;
};
