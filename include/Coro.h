
#pragma once

#include <boost/context/all.hpp>
#include "ThreadPool.h"

#define CORO_STACK_SIZE 1024*32

class Coro {
public:
	static Coro* current();

	Coro(std::function<void()> routine, ThreadPool* threadPool = ThreadPool::current());
	Coro(const Coro& other) = delete;
	Coro(Coro&& other);
	~Coro();

	// вызвали yield -> сделали return из resume
	void yield();
	void schedule();
	void executeSerially(std::function<void()> routine);

	template <typename Exception>
	Exception getException() {
		try {
			std::rethrow_exception(_exception);
		}
		catch (Exception& exception) {
			return exception;
		}
	}
	template <typename Exception>
	void setException(Exception exception) {
		try {
			throw exception;
		}
		catch (...) {
			_exception = std::current_exception();
		}
	}

private:
	// если это первый вызов resume - то входим в routine
	// иначе:
	// вызвали resume -> сделали return из yield
	void resume();
	static void run(intptr_t);
	void doRun();

	std::function<void()> _routine;
	ThreadPool* _threadPool;
	boost::asio::io_service::strand _strand;
	std::vector<unsigned char> _stack;
	boost::context::fcontext_t _context, _savedContext;
	std::exception_ptr _exception;
	bool _isDone;
};
