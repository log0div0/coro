
#include "CoroPool.h"
#include "ThreadPool.h"

CoroPool::CoroPool() {
}

CoroPool::~CoroPool() {
	join();
}

void CoroPool::exec(std::function<void()> routine) {
	auto coro = new Coro([=] {
		routine();

		auto coro = Coro::current();

		coro->strand()->post([=]() {
			std::set<std::shared_ptr<Task>> tasks;
			{
				std::unique_lock<std::mutex> lock(_mutex);

				_coros.erase(coro);

				if (_coros.empty()) {
					// переместим список корутин в стек, на случай, если одна из этих корутин
					// уничтожит CoroPool
					tasks = std::move(_tasks);
				}
			}
			delete coro;
			for (auto& task: tasks) {
				(*task)();
			}
		});
	});

	{
		std::lock_guard<std::mutex> lock(_mutex);
		_coros.insert(coro);
	}

	coro->strand()->post([=] {
		coro->resume();
	});
}

void CoroPool::join() {
	auto task = std::make_shared<Task>();
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
		task->cancel();
		throw;
	}
}

static CoroPool pool;

void Exec(std::function<void()> routine) {
	pool.exec(std::move(routine));
}

void Join() {
	pool.join();
}
