
#include "coro/CoroMain.h"
#include "coro/IoService.h"
#include "coro/Coro.h"

void CoroMain(std::function<void()> routine) {
	IoService ioService;
	Coro coro(std::move(routine));
	ioService.post([&] {
		coro.resume();
	});
	ioService.run();
}