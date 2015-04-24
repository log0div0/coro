
#pragma once

#include <boost/context/all.hpp>
#include "ThreadPool.h"

#define CORO_STACK_SIZE 1024*32

class CancelError: public std::runtime_error {
public:
	CancelError(): std::runtime_error("CancelError") {}
};

class Coro {
public:
	static Coro* current();

	Coro(std::function<void()> routine, ThreadPool* threadPool = ThreadPool::current());
	Coro(const Coro& other) = delete;
	Coro(Coro&& other);
	~Coro();

	// если это первый вызов resume - то входим в routine
	// иначе:
	// вызвали resume -> сделали return из yield
	void resume();
	// вызвали yield -> сделали return из resume
	void yield();

	boost::asio::io_service::strand* strand() {
		return &_strand;
	}

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

	void cancel() {
		this->strand()->post([=] {
			setException(CancelError());
			resume();
		});
	}

private:
	static void run(intptr_t);
	void doRun();

	std::function<void()> _routine;
	boost::asio::io_service::strand _strand;
	std::vector<unsigned char> _stack;
	boost::context::fcontext_t _context, _savedContext;
	std::exception_ptr _exception;
	bool _isDone;
};

class CoroTask: public std::enable_shared_from_this<CoroTask> {
public:
	bool schedule() {
		if (_coro) {
			_coro->strand()->post([task = shared_from_this()] {
				task->execute();
			});
			return true;
		} else {
			return false;
		}
	}

	void execute() {
		if (_coro) {
			_coro->resume();
		}
	}

	void preventExecution() {
		_coro = nullptr;
	}

private:
	Coro* _coro = Coro::current();
};

