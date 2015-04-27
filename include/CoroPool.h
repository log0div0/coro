
#pragma once

#include "Coro.h"
#include <set>

class CoroPool {
public:
	CoroPool();
	~CoroPool();
	Coro* exec(std::function<void()> routine);
	void join();
	template <typename Exception>
	void join(const Exception& exception) {
		for (auto coro: _execCoros) {
			coro->resume(exception);
		}
		join();
	}


private:
	void onCoroDone(Coro* coro);

	std::set<Coro*> _execCoros, _joinCoros;
};
