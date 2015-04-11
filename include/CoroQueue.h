
#pragma once

#include "Coro.h"
#include <mutex>
#include <queue>

template <typename T>
class CoroQueue {
public:
	T pop() {
		while (true) {
			{
				std::unique_lock<std::mutex> lock(_mutex);

				if (_dataQueue.size()) {
					T t = std::move(_dataQueue.front());
					_dataQueue.pop();
					return t;
				}

				_coroQueue.push(Coro::current());
			}
			Coro::current()->yield();
		}
	}

	template <typename U>
	void push(U&& u) {
		std::unique_lock<std::mutex> lock(_mutex);

		_dataQueue.push(std::forward<U>(u));

		if (_coroQueue.size()) {
			_coroQueue.front()->schedule();
			_coroQueue.pop();
		}
	}


private:
	std::mutex _mutex;
	std::queue<T> _dataQueue;
	std::queue<Coro*> _coroQueue;
};
