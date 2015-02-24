
#pragma once

#include "CoroQueue.h"

class CoroPool {
public:
	uint64_t fork(const std::function<void()>& routine);
	uint64_t wait();

private:
	std::mutex _mutex;
	uint64_t _coroCounter;
	std::map<uint64_t, Coro> _coros;
	CoroQueue<uint64_t> _doneCoros;
};
