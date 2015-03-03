
#pragma once

#include <boost/asio.hpp>
#include <thread>

class ThreadPool {
public:
	static ThreadPool* current();

	ThreadPool(size_t threadCount);
	~ThreadPool();

	void schedule(std::function<void()> function);
	/// ждёт, когда потокам станет нечего делать
	void sync();

	boost::asio::io_service& ioService();

protected:
	void startThreads();
	void stopThreads();

private:
	boost::asio::io_service _ioService;
	std::unique_ptr<boost::asio::io_service::work> _work;
	size_t _threadCount;
	std::vector<std::thread> _threads;
};