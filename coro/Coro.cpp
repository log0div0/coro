
#include "coro/Coro.h"
#include "coro/ThreadLocal.h"
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
	  _isDone(false), _yieldNoThrow(false)
{
	_context = boost::context::make_fcontext(_stack.sp, _stack.size, Run);
#if BOOST_VERSION >= 105600
	_savedContext = nullptr;
#endif
}

Coro::~Coro() {
	assert(_isDone);
#ifdef _DEBUG
	if (_exceptions.size()) {
		std::string what;
		for (auto exception: _exceptions) {
			what += "\n";
			try {
				std::rethrow_exception(exception);
			}
			catch (const std::exception& error) {
				what += error.what();
			}
			catch (const CancelError&) {
				what += "CancelError";
			}
			catch (...) {
				what += "...";
			}
		}
		printf("Coro::~Coro: unhandled exceptions: %s", what.c_str());
	}
#endif
}

void Coro::resume() {
	if (_isDone) {
#ifdef _DEBUG
		printf("Coro::resume: coro is done already\n");
#endif
		return;
	}
	auto coro = t_coro;
	t_coro = this;
#if BOOST_VERSION >= 105600
	assert(_context && !_savedContext);
	boost::context::jump_fcontext(&_savedContext, _context, reinterpret_cast<intptr_t>(this));
	_savedContext = nullptr;
#else
	boost::context::jump_fcontext(&_savedContext, _context, reinterpret_cast<intptr_t>(this));
#endif
	t_coro = coro;
}

void Coro::resume(std::exception_ptr exception) {
	assert(exception);
	_exceptions.push_back(exception);
	if (_yieldNoThrow) {
		return;
	}
	resume();
}

void Coro::yield() {
	if (!_isDone) {
		throwException();
	}

#if BOOST_VERSION >= 105600
	assert(!_context && _savedContext);
	boost::context::jump_fcontext(&_context, _savedContext, 0);
	_context = nullptr;
#else
	boost::context::jump_fcontext(_context, &_savedContext, 0);
#endif

	throwException();
}

void Coro::yieldNoThrow() {
	_yieldNoThrow = true;
	yield();
	_yieldNoThrow = false;
}

void Coro::cancel() {
	resume(CancelError());
}

void Coro::throwException() {
	if (_yieldNoThrow) {
		return;
	}
	if (_exceptions.size()) {
		auto exception = _exceptions.front();
		_exceptions.pop_front();
		assert(exception);
		std::rethrow_exception(exception);
	}
}

void Coro::run() {
#if BOOST_VERSION >= 105600
	_context = nullptr;
#endif
	try
	{
		_routine();
	}
	catch (const CancelError&) {
		// do nothing
	}
	catch (...)
	{
		auto exception = std::current_exception();
		assert(exception);
		_exceptions.push_front(exception);
	}
	_isDone = true;
	yield();
}
