
#include "Coro.h"
#include "Common.h"
#include <cassert>

THREAD_LOCAL Coro* t_coro = nullptr;

void Yield() {
	t_coro->yield();
}

void Yield(std::function<void()> callMeJustAfterYield) {
	t_coro->yield(callMeJustAfterYield);
}

Coro* Coro::current() {
	return t_coro;
}

Coro::Coro(std::function<void()> routine, std::function<void()> onDone)
	: _routine(std::move(routine)),
	  _onDone(std::move(onDone)),
	  _isDone(false),
	  _stack(CORO_STACK_SIZE)
{
	_context = boost::context::make_fcontext(&_stack.back(), _stack.size(), &run);
	_savedContext = nullptr;
}

Coro::Coro(Coro&& other)
	: _routine(std::move(other._routine)),
	  _onDone(std::move(other._onDone)),
	  _callMeJustAfterYield(std::move(other._callMeJustAfterYield)),
	  _isDone(other._isDone),
	  _stack(std::move(other._stack)),
	  _context(std::move(other._context)),
	  _savedContext(std::move(other._savedContext)),
	  _exception(std::move(other._exception))
{
	other._routine = nullptr;
	other._onDone = nullptr;
	other._callMeJustAfterYield = nullptr;
	other._isDone = false;
	other._stack.clear();
	other._context = nullptr;
	other._savedContext = nullptr;
	other._exception = std::exception_ptr();
}

Coro::~Coro() {
}

void Coro::resume()
{
	std::lock_guard<std::mutex> lock(_mutex);
	Coro* temp = this;
	std::swap(temp, t_coro);
	// если это первый вызов jump_fcontext - то прыгаем в Coro::run(intptr_t)
	// при последующих вызовах - выпрыгиваем из jump_fcontext, который ниже
	assert(_savedContext || _context);
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

bool Coro::operator==(const Coro& other) const {
	return this == &other;
}

bool Coro::operator<(const Coro& other) const {
	return this < &other;
}

void Coro::replaceDoneCallback(std::function<void()> onDone) {
	_onDone = std::move(onDone);
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
	catch (...)
	{
		_exception = std::current_exception();
	}
	_isDone = true;
	yield();
}
