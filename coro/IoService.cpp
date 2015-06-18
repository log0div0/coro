
#include "coro/IoService.h"
#include "coro/ThreadLocal.h"

THREAD_LOCAL IoService* t_ioService = nullptr;

IoService* IoService::current() {
	return t_ioService;
}

void IoService::post(std::function<void()> routine) {
	_impl.post(std::move(routine));
}

void IoService::run() {
	t_ioService = this;
	_impl.run();
	t_ioService = nullptr;
}

IoService::operator boost::asio::io_service& () {
	return _impl;
}
