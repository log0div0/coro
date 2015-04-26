
#include "Coro.h"
#include "ThreadLocal.h"
#include <cassert>

THREAD_LOCAL Coro* t_coro = nullptr;

void Run(intptr_t coro) {
	reinterpret_cast<Coro*>(coro)->run();
}

Coro* Coro::current() {
	return t_coro;
}

Coro::Coro(std::function<void()> routine)
	: _routine(std::move(routine)),
	  _stack(CORO_STACK_SIZE),
	  _isDone(false)
{
	_context = boost::context::make_fcontext(&_stack.back(), _stack.size(), Run);
	_savedContext = nullptr;
	_exception = nullptr;
}

Coro::~Coro() {
	assert(_isDone);
}

void Coro::resume() {
	assert(!_isDone);
	assert(_context && !_savedContext);
	auto coro = t_coro;
	t_coro = this;
	boost::context::jump_fcontext(&_savedContext, _context, reinterpret_cast<intptr_t>(this));
	_savedContext = nullptr;
	t_coro = coro;
}

void Coro::resume(std::exception_ptr exception) {
	_exception = exception;
	resume();
}

void Coro::yield() {
	assert(!_context && _savedContext);
	boost::context::jump_fcontext(&_context, _savedContext, 0);
	_context = nullptr;

	if (_exception) {
		auto exception = _exception;
		_exception = nullptr;
		std::rethrow_exception(exception);
	}
}

void Coro::cancel() {
	resume(CancelError());
}

void Coro::run() {
	_context = nullptr;
	try
	{
		_routine();
	}
	catch (const CancelError& error) {
		// do nothing
	}
	catch (const std::exception& error) {
		#ifndef NDEBUG
			printf("Uncatched exception: %s\n", error.what());
		#endif
	}
	catch (...)
	{
		#ifndef NDEBUG
			printf("Uncatched exception!!!!\n");
		#endif
	}
	_isDone = true;
	yield();
}
