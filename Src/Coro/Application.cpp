
#include "coro/Application.h"
#include <ObjBase.h>

namespace coro {

Application::Application(const std::function<void()>& main): _strand(&_ioService),
	_coro(main)
{
	_strand.post([=] {
		_coro.start();
	});
}

Application::~Application() {
	for (auto& thread: _threads) {
		thread.join();
	}
}

void Application::run() {
	CoInitializeEx(NULL, COINIT_MULTITHREADED);
	_ioService.run();
	CoUninitialize();
}

void Application::runAsync(size_t threadsCount) {
	_threads.resize(threadsCount);
	for (auto& thread: _threads) {
		thread = std::thread([&] {
			CoInitializeEx(NULL, COINIT_MULTITHREADED);
			_ioService.run();
			CoUninitialize();
		});
	}
}

void Application::cancel() {
	_strand.post([=] {
		_coro.cancel();
	});
}

}