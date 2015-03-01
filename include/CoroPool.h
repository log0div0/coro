
#pragma once

#include "Coro.h"
#include <mutex>
#include <set>
#include <queue>

class CoroPool {
public:
	static CoroPool& global();

	void fork(std::function<void()> routine);
	void join();

private:
	std::mutex _mutex;
	std::set<Coro> _coros;
	std::queue<std::function<void()>> _callOnJoin;
};

void Fork(std::function<void()> routine); //< CoroPool::global