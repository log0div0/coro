
#pragma once

#include <boost/asio.hpp>

class ThreadPool {
public:
	static ThreadPool* current();

	void schedule(std::function<void()> function);
	void run();

	boost::asio::io_service& ioService();

private:
	boost::asio::io_service _ioService;
};