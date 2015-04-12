
#pragma once

#include "Coro.h"
#include <mutex>
#include <queue>

template <typename T>
class CoroQueue {
public:
	T pop() {
		while (true) {
			auto coro = std::make_shared<Coro*>(Coro::current());
			{
				std::unique_lock<std::mutex> lock(_mutex);

				if (_dataQueue.size()) {
					T t = std::move(_dataQueue.front());
					_dataQueue.pop();
					return t;
				}

				_coroQueue.push(coro);
			}
			try {
				Coro::current()->yield();
			}
			catch (...) {
				std::unique_lock<std::mutex> lock(_mutex);
				*coro = nullptr;
				next();
				throw;
			}
		}
	}

	template <typename U>
	void push(U&& u) {
		std::unique_lock<std::mutex> lock(_mutex);
		_dataQueue.push(std::forward<U>(u));
		next();
	}

private:
	void next() {
		while (_coroQueue.size()) {
			auto coro = std::move(_coroQueue.front());
			_coroQueue.pop();
			if (*coro == nullptr) {
				continue;
			}
			(*coro)->strand()->post([coro] {
				if (*coro) {
					(*coro)->resume();
				}
			});
			return;
		}
	}

private:
	std::mutex _mutex;
	std::queue<T> _dataQueue;
	std::queue<std::shared_ptr<Coro*>> _coroQueue;
};
