
#include "SignalSet.h"
#include "IoService.h"

SignalSet::SignalSet(const std::initializer_list<int32_t>& signals)
	: _handler(*IoService::current())
{
	for (auto signal: signals) {
		_handler.add(signal);
	}
}

int32_t SignalSet::wait() {
	AsioTask2<int32_t> task;
	_handler.async_wait(task.callback());
	return task.wait(_handler);
}