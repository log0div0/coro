
#pragma once

#include "Task.h"
#include <mutex>
#include <functional>
#include <queue>

class CoroMutex {
public:
	void lock();
	void unlock();

private:
	bool _isLocked = false;
	std::mutex _mutex;
	std::queue<Task> _taskQueue;
};