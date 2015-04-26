
#pragma once

#include <functional>
#include <vector>
#include <boost/context/all.hpp>

#define CORO_STACK_SIZE 1024 * 32

struct CancelError {};

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

	void cancel();

private:
	std::function<void()> _routine;
	std::vector<uint8_t> _stack;
	boost::context::fcontext_t _context, _savedContext;
	std::exception_ptr _exception;
	bool _isDone;

public:
	void run();
};
