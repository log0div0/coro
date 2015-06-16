
#pragma once

#include "coro/Coro.h"
#include <mutex>
#include <list>

class Mutex {
public:
	void lock();
	void unlock();

private:
	Coro* _owner = nullptr;
	std::list<Coro*> _coros;
};