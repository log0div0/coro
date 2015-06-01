
#pragma once

#include <functional>
#include <vector>
#include <memory>
#include <boost/context/all.hpp>
#include <boost/coroutine/stack_context.hpp>
#include <boost/coroutine/protected_stack_allocator.hpp>
#include <deque>
#include <boost/version.hpp>

struct CancelError {};

#ifdef _DEBUG
#define CORO_STACK_SIZE 1024 * 1024 * 4
#else
#define CORO_STACK_SIZE 1024 * 32
#endif

struct CoroStack: public boost::coroutines::stack_context {
	CoroStack() {
		boost::coroutines::protected_stack_allocator().allocate(*this, CORO_STACK_SIZE);
	}
	~CoroStack() {
		boost::coroutines::protected_stack_allocator().deallocate(*this);
	}
};

class Coro {
public:
	static Coro* current();

	Coro(std::function<void()> routine);
	~Coro();

	void resume();
	void resume(std::exception_ptr exception);
	template <typename Exception>
	void resume(Exception exception) {
		resume(std::make_exception_ptr(exception));
	}
	void yield();
	void yieldNoThrow();

	void cancel();

	const std::deque<std::exception_ptr>& exceptions() const {
		return _exceptions;
	}

private:
	void throwException();

	std::function<void()> _routine;
	std::unique_ptr<CoroStack, std::function<void(CoroStack*)>&> _stack;
#if BOOST_VERSION >= 105600
	boost::context::fcontext_t _context, _savedContext;
#else
	boost::context::fcontext_t* _context;
	boost::context::fcontext_t _savedContext;
#endif
	std::deque<std::exception_ptr> _exceptions;
	bool _isDone, _yieldNoThrow;

public:
	void run();
};
