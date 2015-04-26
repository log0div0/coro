
#pragma once

#include <boost/asio.hpp>

class IoService {
public:
	static IoService* current();

	void post(std::function<void()> routine);
	void run();

	operator boost::asio::io_service& ();

private:
	boost::asio::io_service _impl;
};