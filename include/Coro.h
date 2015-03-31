
#pragma once

#include <functional>
#include <vector>
#include <boost/context/all.hpp>

#define CORO_STACK_SIZE 1024*32

void CoroYield();
void CoroYield(std::function<void()> callMeJustAfterYield);

struct Coro
{
	static Coro* current();

	// onDone вызывается когда routine отработает (или выбросит исключение) И! когда мы уже выйдем
	// из корутины
	Coro(std::function<void()> routine,
		std::function<void()> onDone = std::function<void()>());
	Coro(Coro&& other);
	~Coro();

	// если это первый вызов resume - то входим в routine в новом стеке
	// при последующих вызовах работает так:
	// вызвали resume -> сделали return из yield в стеке корутины
	void resume();
	// вызвали yield -> сделали return из resume, стек вернули на место
	void yield();
	// бросить исключение внутри корутины
	template <typename Exception>
	void resume(Exception exception) {
		try {
			throw exception;
		}
		catch (...) {
			_exception = std::current_exception();
		}
		resume();
	}
	// callMeJustAfterYield вызывается после yield, но перед возвратом из resume
	void yield(std::function<void()> callMeJustAfterYield);

	// если routine выбросит исключение, то забрать его можно здесь
	std::exception_ptr exception();

	// for std::set & std::map
	bool operator==(const Coro& other) const;
	bool operator<(const Coro& other) const;

	void replaceDoneCallback(std::function<void()> onDone);

private:
	Coro(const Coro& other);

	static void run(intptr_t);
	void doRun();

	std::function<void()> _routine, _onDone, _callMeJustAfterYield;
	bool _isDone;
	std::vector<unsigned char> _stack;
	boost::context::fcontext_t _context, _savedContext;
	std::exception_ptr _exception;
};
