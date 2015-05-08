
#pragma once

#include <functional>
#include <vector>
#include <boost/context/all.hpp>
#include <queue>

#ifdef _DEBUG
#define CORO_STACK_SIZE 1024 * 1024 * 2
#else
#define CORO_STACK_SIZE 1024 * 32
#endif

struct CancelError {};

class Coro {
public:
	static Coro* current();

	Coro(std::function<void()> routine);
	~Coro();

	void resume();
	template <typename Exception>
	void resume(Exception exception) {
		throwOnResumeOrYield(std::make_exception_ptr(exception));
		resume();
	}
	void yield();

	void throwOnResumeOrYield(std::exception_ptr exception);
	void cancel();

private:
	void throwException();

	std::function<void()> _routine;
	std::vector<uint8_t> _stack;
	boost::context::fcontext_t _context, _savedContext;
	std::queue<std::exception_ptr> _exceptions;
	bool _isDone;

public:
	void run();
};
