
#include "Coro.h"

#ifdef _MSC_VER
#define THREAD_LOCAL __declspec(thread)
#else
#define THREAD_LOCAL thread_local
#endif

THREAD_LOCAL Coro* t_coro = nullptr;

void yield() {
	t_coro->yield();
}

void yield(std::function<void()> callMeJustAfterYield) {
	t_coro->yield(callMeJustAfterYield);
}

Coro* Coro::current() {
	return t_coro;
}

Coro::Coro(std::function<void()> program, std::function<void()> onDone)
	: _program(std::move(program)), _onDone(onDone), _isDone(false),
	  _stack(CORO_STACK_SIZE)
{
	_context = boost::context::make_fcontext(&_stack.back(), _stack.size(), &run);
	_savedContext = nullptr;
}

Coro::~Coro() {
}

void Coro::resume()
{
	Coro* temp = this;
	std::swap(temp, t_coro);
	// если это первый вызов jump_fcontext - то прыгаем в Coro::run(intptr_t)
	// при последующих вызовах - выпрыгиваем из jump_fcontext, который ниже
	boost::context::jump_fcontext(&_savedContext, _context, 0);
	std::swap(temp, t_coro);
	if (_callMeJustAfterYield) {
		_callMeJustAfterYield();
		_callMeJustAfterYield = nullptr;
	}
	if (_onDone && _isDone) {
		_onDone();
	}
}

void Coro::yield()
{
	// выпрыгиваем из jump_fcontext, который выше
	boost::context::jump_fcontext(&_context, _savedContext, 0);
}

void Coro::yield(std::function<void()> callMeJustAfterYield) {
	_callMeJustAfterYield = std::move(callMeJustAfterYield);
	yield();
}

std::exception_ptr Coro::exception() {
	return _exception;
}

void Coro::run(intptr_t)
{
	t_coro->doRun();
}

void Coro::doRun()
{
	try
	{
		_program();
	}
	catch (...)
	{
		_exception = std::current_exception();
	}
	_isDone = true;
	yield();
}
