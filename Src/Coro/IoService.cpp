
#include "Coro/IoService.h"
#ifdef _MSC_VER
#include "Coro/FiberWindows.h"
#endif
#ifdef __GNUC__
#include "Coro/FiberLinux.h"
#endif

namespace coro {

thread_local IoService* t_ioService = nullptr;

IoService* IoService::current() {
	return t_ioService;
}

void IoService::run() {
	Fiber::initialize();
	t_ioService = this;
	_impl.run();
	t_ioService = nullptr;
	Fiber::deinitialize();
}

}