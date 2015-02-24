
#include "ThreadPool.h"
#include "Common.h"


THREAD_LOCAL ThreadPool* t_threadPool = nullptr;


ThreadPool* ThreadPool::current() {
	return t_threadPool;
}

void ThreadPool::schedule(std::function<void()> function) {
	_ioService.post(function);
}

void ThreadPool::run() {
	t_threadPool = this;
	_ioService.run();
}

boost::asio::io_service& ThreadPool::ioService() {
	return _ioService;
}