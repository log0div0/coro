
#include "CoroPool.h"

uint64_t CoroPool::fork(const std::function<void()>& routine) {
	std::lock_guard<std::mutex> lock(_mutex);
	uint64_t coroId = _coroCounter++;
	auto result = _coros.emplace(std::piecewise_construct,
		std::forward_as_tuple(coroId),
		std::forward_as_tuple(routine, [&, coroId]() { _doneCoros.push(coroId); })
	);
	assert(result.second);
	ThreadPool::current()->schedule([&coro = result.first->second]() {
		coro.resume();
	});
	return coroId;
}

uint64_t CoroPool::wait() {
	uint64_t coroId = _doneCoros.pop();
	std::lock_guard<std::mutex> lock(_mutex);
	_coros.erase(coroId);
	return coroId;
}