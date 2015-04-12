
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
			{
				std::unique_lock<std::mutex> lock(_mutex);

				_execCoros.erase(coro);

				if (_execCoros.empty()) {
					for (auto& coro: _joinCoros) {
						if (*coro) {
							(*coro)->strand()->post([coro] {
								if (*coro) {
									(*coro)->resume();
								}
							});
						}
					}
					_joinCoros.clear();
				}

			}
			delete coro;
		});
	});

	{
		std::lock_guard<std::mutex> lock(_mutex);
		_execCoros.insert(coro);
	}

	coro->strand()->post([=] {
		coro->resume();
	});
}

void CoroPool::join() {
	auto coro = std::make_shared<Coro*>(Coro::current());
	{
		std::unique_lock<std::mutex> lock(_mutex);

		if (_execCoros.empty()) {
			return;
		}

		_joinCoros.insert(coro);
	}
	try {
		Coro::current()->yield();
	}
	catch (...) {
		std::unique_lock<std::mutex> lock(_mutex);
		*coro = nullptr;
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
