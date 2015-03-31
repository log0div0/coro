
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
		std::unique_lock<std::mutex> lock(_mutex);

		_coros.erase(coro);

		std::queue<Task> callOnJoin;
		if (_coros.empty()) {
			callOnJoin = std::move(_callOnJoin);
		}

		lock.unlock();

		while (!callOnJoin.empty()) {
			callOnJoin.front()();
			callOnJoin.pop();
		}
	});

	ThreadPool::current()->schedule([&]() {
		coro.resume();
	});
}

void CoroPool::join() {
	std::unique_lock<std::mutex> lock(_mutex);

	if (_coros.empty()) {
		return;
	}

	_callOnJoin.push(Task::current());

	CoroYield([&]() {
		lock.unlock();
	});
}

static CoroPool pool;

void Exec(std::function<void()> routine) {
	pool.exec(std::move(routine));
}

void Join() {
	pool.join();
}
