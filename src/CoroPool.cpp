
#include "CoroPool.h"
#include "ThreadPool.h"

void CoroPool::fork(std::function<void()> routine) {
	std::lock_guard<std::mutex> lock(_mutex);

	auto result = _coros.emplace(std::move(routine));
	Coro& coro = const_cast<Coro&>(*result.first);
	assert(result.second);

	coro.replaceDoneCallback([&]() {
		ThreadPool::current()->schedule([&]() {
			std::lock_guard<std::mutex> lock(_mutex);
			_coros.erase(coro);
			if (_coros.empty()) {
				if (_onJoin) {
					_onJoin();
					_onJoin = nullptr;
				}
			}
		});
	});

	ThreadPool::current()->schedule([&]() {
		coro.resume();
	});
}

void CoroPool::join() {
	{
		std::lock_guard<std::mutex> lock(_mutex);
		if (_coros.empty()) {
			return;
		}
		_onJoin = [threadPool = ThreadPool::current(), coro = Coro::current()] {
			threadPool->schedule([coro]() {
				coro->resume();
			});
		};
	}
	Coro::current()->yield();
}