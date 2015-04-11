
#include "Task.h"

Task::Task(): _coro(Coro::current())
{
	_routine = [&] {
		_coro->resume();
	};
}

Task::Task(std::function<void()> routine): _coro(Coro::current()), _routine(std::move(routine))
{
}

void Task::assign(std::function<void()> routine) {
	_routine = std::move(routine);
}

void Task::operator()() {
	_coro->strand()->post([self = shared_from_this()] {
		if (self->_routine) {
			(self->_routine)();
		}
	});
}