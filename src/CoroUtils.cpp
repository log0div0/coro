
#include "CoroUtils.h"
#include "ThreadPool.h"
#include "Coro.h"
#include <atomic>

void Parallel(const std::initializer_list<std::function<void()>>& routines,
	const std::function<void(const std::exception&)>& errorHandler)
{
	ThreadPool& threadPool = *ThreadPool::current();
	Coro& oldCoro = *Coro::current();

	std::atomic_size_t corosCount(routines.size());
	// этот callback возобновит текущую корутину, когда все дочерние корутины завершатся
	auto onCoroDone = [&]() {
		if (--corosCount == 0) {
			threadPool.schedule([&oldCoro]() {
				oldCoro.resume();
			});
		}
	};

	std::vector<Coro> coros;
	coros.reserve(corosCount);
	for (auto routine: routines) {
		coros.emplace_back(std::move(routine), onCoroDone);
	};

	// запланируем запуск дочерних корутин, когда выйдем из текущей корутины
	oldCoro.yield([&] {
		for (auto& newCoro: coros) {
			threadPool.schedule([&newCoro]() {
				newCoro.resume();
			});
		}
	});

	// обработка исключений
	for (auto& newCoro: coros) {
		if (newCoro.exception() != std::exception_ptr()) {
			try {
				std::rethrow_exception(newCoro.exception());
			} catch (const std::exception& exception) {
				if (errorHandler) {
					errorHandler(exception);
				}
			}
		}
	}
}
