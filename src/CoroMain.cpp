
#include "CoroMain.h"
#include "IoService.h"
#include "Coro.h"

void CoroMain(std::function<void()> routine) {
	IoService ioService;
	Coro coro(std::move(routine));
	ioService.post([&] {
		coro.resume();
	});
	ioService.run();
}