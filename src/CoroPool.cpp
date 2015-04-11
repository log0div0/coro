
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

		Coro::current()->executeSerially([&, coro = Coro::current()]() {
			std::set<Coro*> joinCoros;
			{
				std::unique_lock<std::mutex> lock(_mutex);
				_execCoros.erase(coro);
				if (_execCoros.empty()) {
					// переместим список корутин в стек, на случай, если одна из этих корутин
					// уничтожит CoroPool
					joinCoros = std::move(_joinCoros);
				}
			}
			delete coro;
			for (auto coro: joinCoros) {
				coro->schedule();
			}
		});
	});

	{
		std::lock_guard<std::mutex> lock(_mutex);
		_execCoros.insert(coro);
	}

	coro->schedule();
}

void CoroPool::join() {
	{
		std::unique_lock<std::mutex> lock(_mutex);

		if (_execCoros.empty()) {
			return;
		}

		_joinCoros.insert(Coro::current());
	}
	Coro::current()->yield();
}

static CoroPool pool;

void Exec(std::function<void()> routine) {
	pool.exec(std::move(routine));
}

void Join() {
	pool.join();
}
