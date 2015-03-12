
#include "CoroPool.h"
#include "ThreadPool.h"

CoroPool::CoroPool() {
}

CoroPool::~CoroPool() {
	join();
}

void CoroPool::exec(std::function<void()> routine) {
	std::lock_guard<std::mutex> lock(_mutex);

	auto result = _coros.emplace(std::move(routine));
	Coro& coro = const_cast<Coro&>(*result.first);
	assert(result.second);

	coro.replaceDoneCallback([&]() {
		ThreadPool::current()->schedule([&]() {
			std::queue<std::function<void()>> callOnJoin;
			{
				std::lock_guard<std::mutex> lock(_mutex);
				_coros.erase(coro);
				if (_coros.empty()) {
					callOnJoin = std::move(_callOnJoin);
				}
			}
			while (!callOnJoin.empty()) {
				callOnJoin.front()();
				callOnJoin.pop();
			}
		});
	});

	ThreadPool::current()->schedule([&]() {
		coro.resume();
	});
}

void CoroPool::join() {
	_mutex.lock();
	if (_coros.empty()) {
		_mutex.unlock();
		return;
	}
	_callOnJoin.push([threadPool = ThreadPool::current(), coro = Coro::current()] {
		threadPool->schedule([=]() {
			coro->resume();
		});
	});
	Coro::current()->yield([&]() {
		_mutex.unlock();
	});
}

static CoroPool pool;

void Exec(std::function<void()> routine) {
	pool.exec(routine);
}

void Join() {
	pool.join();
}