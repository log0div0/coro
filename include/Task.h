
#pragma once

#include "Coro.h"

class Task: public std::enable_shared_from_this<Task> {
public:
	Task();
	Task(std::function<void()> routine);

	void assign(std::function<void()> routine);
	void cancel() {
		assign(std::function<void()>());
	}
	void operator()();

private:
	Coro* _coro;
	std::function<void()> _routine;
};