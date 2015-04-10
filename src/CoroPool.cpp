
#include "CoroPool.h"
#include "ThreadPool.h"

CoroPool::CoroPool() {
}

CoroPool::~CoroPool() {
	join();
}

void CoroPool::exec(std::function<void()> routine) {
	auto coro = new Coro([&, routine = std::move(routine)]() {
		routine();

		std::unique_lock<std::mutex> lock(_mutex);

		_running.erase(Coro::current());
		_finished.insert(Coro::current());

		if (_running.empty()) {
			for (auto i: _waiting) {
				i->schedule();
			}
			_waiting.clear();
		}
	});

	{
		std::lock_guard<std::mutex> lock(_mutex);
		_running.insert(coro);
	}

	coro->schedule();
}

void CoroPool::join() {
	{
		std::unique_lock<std::mutex> lock(_mutex);

		if (_running.empty()) {
			return;
		}

		_waiting.insert(Coro::current());
	}
	Coro::current()->yield();
	{
		std::unique_lock<std::mutex> lock(_mutex);
		cleanup();
	}
}

void CoroPool::cleanup() {
	for (auto i: _finished) {
		delete i;
	}
	_finished.clear();
}

static CoroPool pool;

void Exec(std::function<void()> routine) {
	pool.exec(std::move(routine));
}

void Join() {
	pool.join();
}
