
#include "coro/Application.h"
#ifdef _MSC_VER
#include <ObjBase.h>
#endif

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
		try{
			thread.join();
		}
		catch (const std::exception&){
			// Подавление ошибок
		}
	}
}

void Application::run() {
#ifdef _MSC_VER
	CoInitializeEx(NULL, COINIT_MULTITHREADED);
#endif
	_ioService.run();
#ifdef _MSC_VER
	// https://support.microsoft.com/ru-ru/help/293278/prb-problems-when-you-call-coinitialize-and-couninitialize-repeatedly-in-multithreaded-apartment
	// По рекомендациям от microsoft для многопоточного COM нужно однократно вызывать CoInitializeEx в потоке и
	// не вызывать CoUninitialize() - ресурсы вернутся в систему при завершении программы
#endif
}

void Application::runAsync(size_t threadsCount) {
	_threads.resize(threadsCount);
	for (auto& thread: _threads) {
		thread = std::thread([=] { run(); });
	}
}

void Application::cancel() {
	_strand.post([=] {
		_coro.cancel();
	});
}

void Application::propagateException(std::exception_ptr exception) {
	_strand.post([=] {
		_coro.propagateException(exception);
	});
}

}