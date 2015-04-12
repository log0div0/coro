
#pragma once

#include "Coro.h"

class Task: public std::enable_shared_from_this<Task> {
public:
	Task();

	void operator()();
	void cancel();

private:
	Coro* _coro;
};