
#pragma once

#include "Task.h"
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
	std::set<Coro*> _coros;
	std::set<std::shared_ptr<Task>> _tasks;
};

void Exec(std::function<void()> routine);
void Join();