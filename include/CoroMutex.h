
#pragma once

#include "Coro.h"
#include <mutex>
#include <functional>
#include <queue>

class CoroMutex {
public:
	void lock();
	void unlock();

private:
	void next();

private:
	bool _isLocked = false;
	std::mutex _mutex;
	std::queue<std::shared_ptr<Coro*>> _coroQueue;
};