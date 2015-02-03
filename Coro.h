
#pragma once

#include <functional>
#include <vector>
#include <boost/context/all.hpp>

#define CORO_STACK_SIZE 1024*32

struct Coro
{
	static Coro* current();

	Coro(std::function<void()> program,
		std::function<void()> onDone = std::function<void()>());
	~Coro();
	void resume();
	void yield();
	void yield(std::function<void()> callMeJustAfterYield);
	std::exception_ptr exception();

private:
	static void run(intptr_t);
	void doRun();

	std::function<void()> _program, _onDone, _callMeJustAfterYield;
	bool _isDone;
	std::vector<unsigned char> _stack;
	boost::context::fcontext_t _context, _savedContext;
	std::exception_ptr _exception;
};
