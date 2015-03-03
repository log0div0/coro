
#include "ThreadPool.h"
#include "Common.h"


THREAD_LOCAL ThreadPool* t_threadPool = nullptr;


ThreadPool::ThreadPool(size_t threadCount): _threadCount(threadCount) {
	startThreads();
}


ThreadPool::~ThreadPool() {
	stopThreads();
}


ThreadPool* ThreadPool::current() {
	return t_threadPool;
}


void ThreadPool::schedule(std::function<void()> function) {
	_ioService.post(function);
}


void ThreadPool::sync() {
	stopThreads();
	startThreads();
}


boost::asio::io_service& ThreadPool::ioService() {
	return _ioService;
}

void ThreadPool::startThreads() {
	_work.reset(new boost::asio::io_service::work(_ioService));
	_threads.reserve(_threadCount);
	for (size_t i = 0; i < _threadCount; ++i) {
		_threads.emplace_back(std::thread([this] {
			t_threadPool = this;
			while (true) {
				_ioService.run();
			}
		}));
	}
}

void ThreadPool::stopThreads() {
	_work.reset();
	for (size_t i = 0; i < _threadCount; ++i) {
		_threads[i].join();
	}
	_threads.clear();
	_ioService.reset();
}