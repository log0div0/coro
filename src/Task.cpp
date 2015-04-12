
#include "Task.h"

Task::Task(): _coro(Coro::current())
{
}

void Task::operator()() {
	if (_coro) {
		_coro->strand()->post([self = shared_from_this()] {
			if (self->_coro) {
				self->_coro->resume();
			}
		});
	}
}

void Task::cancel() {
	_coro = nullptr;
}