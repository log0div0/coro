
#include "ThreadPool.h"
#include "Coro.h"

void ThreadPool::main(std::function<void()> program) {
	Coro coro(program);
	_io_service.post([&] {
		coro.resume();
	});
	_io_service.run();
}

void ThreadPool::parallel(std::initializer_list<std::function<void()>> programs,
	std::function<void(const std::exception&)> errorHandler)
{
	Coro& old_coro = *Coro::current();

	std::atomic_size_t coros_count(programs.size());
	auto onCoroDone = [&]() {
		if (--coros_count == 0) {
			_io_service.post([&] {
				old_coro.resume();
			});
		}
	};

	std::vector<Coro> coros;
	coros.reserve(coros_count);
	for (auto program: programs) {
		coros.emplace_back(program, onCoroDone);
	};

	old_coro.yield([&] {
		for (auto& new_coro: coros) {
			_io_service.post([&new_coro]() {
				new_coro.resume();
			});
		}
	});

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