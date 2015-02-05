
#pragma once

#include <functional>
#include <vector>
#include <boost/context/all.hpp>

#define CORO_STACK_SIZE 1024*32

struct Coro
{
	static Coro* current();

	// onDone вызывается когда program отработает (или выбросит исключение) И! когда мы уже выйдем
	// из корутины
	Coro(std::function<void()> program,
		std::function<void()> onDone = std::function<void()>());
	~Coro();

	// если это первый вызов resume - то входим в program в новом стеке
	// при последующих вызовах работает так:
	// вызвали resume -> сделали return из yield в стеке корутины
	void resume();
	// вызвали yield -> сделали return из resume, стек вернули на место
	void yield();
	// callMeJustAfterYield вызывается после yield, но перед возвратом из resume
	// используется для того, чтобы запланировать выполнение других корутин
	void yield(std::function<void()> callMeJustAfterYield);

	// если program выбросит исключение, то забрать его можно здесь
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
