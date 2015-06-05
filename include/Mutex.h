
#pragma once

#include "Coro.h"
#include <mutex>
#include <list>

class Mutex {
public:
	void lock();
	void unlock();

private:
	Coro* _owner = nullptr;
	std::list<Coro*> _nonOwners;
};