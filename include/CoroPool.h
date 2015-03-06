
#pragma once

#include "Coro.h"
#include <mutex>
#include <set>
#include <queue>

class CoroPool {
public:
	CoroPool();
	~CoroPool();
	void exec(std::function<void()> routine);
	void join();

private:
	std::mutex _mutex;
	std::set<Coro> _coros;
	std::queue<std::function<void()>> _callOnJoin;
};

void Exec(std::function<void()> routine);
void Join();