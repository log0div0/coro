
#pragma once

#include "Coro.h"
#include <mutex>
#include <queue>

template <typename T>
class CoroQueue {
public:
	T pop() {
		{
			std::unique_lock<std::mutex> lock(_mutex);

			if (_size) {
				--_size;
				T t = std::move(_dataQueue.front());
				_dataQueue.pop();
				return t;
			}

			_coroQueue.push(Coro::current());
		}

		Coro::current()->yield();

		{
			std::lock_guard<std::mutex> lock(_mutex);

			T t = std::move(_dataQueue.front());
			_dataQueue.pop();
			return t;
		}
	}

	template <typename U>
	void push(U&& u) {
		std::unique_lock<std::mutex> lock(_mutex);

		_dataQueue.push(std::forward<U>(u));

		if (_coroQueue.size()) {
			_coroQueue.front()->schedule();
			_coroQueue.pop();
		} else {
			++_size;
		}
	}


private:
	std::mutex _mutex;
	size_t _size = 0; //< кол-во элементов, которые ещё никому не нужны
	std::queue<T> _dataQueue;
	std::queue<Coro*> _coroQueue;
};
