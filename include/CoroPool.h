
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
	void cleanup();

	std::mutex _mutex;
	std::set<Coro*> _running, _finished, _waiting;
};

void Exec(std::function<void()> routine);
void Join();