
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
	std::set<Coro*> _execCoros;
	std::set<std::shared_ptr<Coro*>> _joinCoros;
};

void Exec(std::function<void()> routine);
void Join();