
#include "coro/IoService.h"
#include "coro/Fiber.h"

namespace coro {

thread_local IoService* t_ioService = nullptr;

IoService* IoService::current() {
	return t_ioService;
}

void IoService::run() {
	Fiber::convertThreadToFiber();
	t_ioService = this;
	_impl.run();
	t_ioService = nullptr;
	Fiber::convertFiberToThread();
}

}