
#include "ThreadPool.h"
#include "Coro.h"
#include <atomic>

void ThreadPool::main(std::function<void()> program) {
	Coro coro(program);
	_io_service.post([&] {
		coro.resume();
	});
	_io_service.run();
}

void ThreadPool::parallel(const std::initializer_list<std::function<void()>>& programs,
	const std::function<void(const std::exception&)>& errorHandler)
{
	Coro& old_coro = *Coro::current();

	std::atomic_size_t corosCount(programs.size());
	// этот callback возобновит текущую корутину, когда все дочерние корутины завершатся
	auto onCoroDone = [&]() {
		if (--corosCount == 0) {
			_io_service.post([&] {
				old_coro.resume();
			});
		}
	};

	// создаём корутины, сохраняем их в стеке текущей корутины
	std::vector<Coro> coros;
	coros.reserve(corosCount);
	for (auto program: programs) {
		coros.emplace_back(std::move(program), onCoroDone);
	};

	old_coro.yield([&] {
		// запланируем запуск корутин сразу же, как выйдем из текущей корутины
		for (auto& new_coro: coros) {
			_io_service.post([&new_coro]() {
				new_coro.resume();
			});
		}
	});

	// обработка исключений
	for (auto& new_coro: coros) {
		if (new_coro.exception() != std::exception_ptr()) {
			try {
				std::rethrow_exception(new_coro.exception());
			} catch (const std::exception& exception) {
				if (errorHandler) {
					errorHandler(exception);
				}
			}
		}
	}
}

boost::asio::io_service& ThreadPool::ioService() {
	return _io_service;
}