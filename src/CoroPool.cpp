
#include "CoroPool.h"
#include "ThreadPool.h"

CoroPool& CoroPool::global() {
	static CoroPool pool;
	return pool;
}

void CoroPool::exec(std::function<void()> routine) {
	std::lock_guard<std::mutex> lock(_mutex);

	auto result = _coros.emplace(std::move(routine));
	Coro& coro = const_cast<Coro&>(*result.first);
	assert(result.second);

	coro.replaceDoneCallback([&]() {
		ThreadPool::current()->schedule([&]() {
			std::lock_guard<std::mutex> lock(_mutex);
			_coros.erase(coro);
			if (_coros.empty()) {
				while (!_callOnJoin.empty()) {
					_callOnJoin.front()();
					_callOnJoin.pop();
				}
			}
		});
	});

	ThreadPool::current()->schedule([&]() {
		coro.resume();
	});
}

void CoroPool::join() {
	if (_coros.empty()) {
		return;
	}
	_mutex.lock();
	_callOnJoin.push([threadPool = ThreadPool::current(), coro = Coro::current()] {
		threadPool->schedule([=]() {
			coro->resume();
		});
	});
	Coro::current()->yield([&]() {
		_mutex.unlock();
	});
}

void Exec(std::function<void()> routine) {
	CoroPool::global().exec(routine);
}