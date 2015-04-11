
#include "Coro.h"
#include "Common.h"
#include <cassert>
#include <iostream>

THREAD_LOCAL Coro* t_coro = nullptr;

Coro* Coro::current() {
	return t_coro;
}

Coro::Coro(std::function<void()> routine, ThreadPool* threadPool)
	: _routine(std::move(routine)),
	  _threadPool(threadPool),
	  _strand(_threadPool->ioService()),
	  _stack(CORO_STACK_SIZE),
	  _isDone(false)
{
	_context = boost::context::make_fcontext(&_stack.back(), _stack.size(), &run);
	_savedContext = nullptr;
}

Coro::Coro(Coro&& other)
	: _routine(std::move(other._routine)),
	  _threadPool(std::move(other._threadPool)),
	  _strand(std::move(other._strand)),
	  _stack(std::move(other._stack)),
	  _context(std::move(other._context)),
	  _savedContext(std::move(other._savedContext)),
	  _exception(std::move(other._exception)),
	  _isDone(std::move(other._isDone))
{
	other._routine = nullptr;
	other._threadPool = nullptr;
	other._stack.clear();
	other._context = nullptr;
	other._savedContext = nullptr;
	other._exception = std::exception_ptr();
}

Coro::~Coro() {
	assert(_isDone);
}

void Coro::resume()
{
	assert(!_isDone);

	Coro* temp = this;

	std::swap(temp, t_coro);
	assert(_savedContext || _context);
	// если это первый вызов jump_fcontext - то входим в Coro::run(intptr_t)
	// иначе - выпрыгиваем из jump_fcontext, который ниже
	boost::context::jump_fcontext(&_savedContext, _context, 0);
	std::swap(temp, t_coro);
}

void Coro::yield()
{
	// выпрыгиваем из jump_fcontext, который выше
	boost::context::jump_fcontext(&_context, _savedContext, 0);
	if (_exception) {
		std::exception_ptr exception = _exception;
		_exception = nullptr;
		std::rethrow_exception(exception);
	}
}

void Coro::schedule() {
	executeSerially([&]() {
		resume();
	});
}

void Coro::executeSerially(std::function<void()> routine) {
	_threadPool->schedule(_strand.wrap(routine));
}

void Coro::run(intptr_t)
{
	t_coro->doRun();
}

void Coro::doRun()
{
	try
	{
		_routine();
	}
	catch (const std::exception& error) {
#ifndef NDEBUG
		std::cout << "Uncatched exception: " << error.what() << std::endl;
#endif
		_exception = std::current_exception();
	}
	catch (...)
	{
#ifndef NDEBUG
		std::cout << "Uncatched exception!!!!" << std::endl;
#endif
		_exception = std::current_exception();
	}
	_isDone = true;
	yield();
}
