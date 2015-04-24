
#include "CoroPool.h"

CoroPool::CoroPool() {
}

CoroPool::~CoroPool() {
	join();
}

Coro* CoroPool::exec(std::function<void()> routine) {
	auto coro = new Coro([=] {
		routine();

		auto coro = Coro::current();

		coro->strand()->post([=]() {
			{
				std::unique_lock<std::mutex> lock(_mutex);

				_coros.erase(coro);

				if (_coros.empty()) {
					for (auto& task: _tasks) {
						task->schedule();
					}
					_tasks.clear();
				}
			}
			delete coro;
		});
	});

	{
		std::lock_guard<std::mutex> lock(_mutex);
		_coros.insert(coro);
	}

	coro->strand()->post([=] {
		coro->resume();
	});

	return coro;
}

void CoroPool::join() {
	auto task = std::make_shared<CoroTask>();
	{
		std::unique_lock<std::mutex> lock(_mutex);

		if (_coros.empty()) {
			return;
		}

		_tasks.insert(task);
	}
	try {
		Coro::current()->yield();
	}
	catch (...) {
		std::unique_lock<std::mutex> lock(_mutex);
		task->preventExecution();
		throw;
	}
	{
		std::unique_lock<std::mutex> lock(_mutex);
	}
}

static CoroPool pool;

void Exec(std::function<void()> routine) {
	pool.exec(std::move(routine));
}

void Join() {
	pool.join();
}
