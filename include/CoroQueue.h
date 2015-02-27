
#pragma once

#include "ThreadPool.h"
#include "Coro.h"
#include <mutex>
#include <queue>

template <typename T>
class CoroQueue {
public:
	T pop() {
		{
			std::lock_guard<std::mutex> lock(_mutex);

			if (_size) {
				--_size;
				T t = _dataQueue.front();
				_dataQueue.pop();
				return t;
			}

			_coroQueue.push([threadPool = ThreadPool::current(), coro = Coro::current()] {
				threadPool->schedule([coro]() {
					coro->resume();
				});
			});
		}

		Coro::current()->yield();

		std::lock_guard<std::mutex> lock(_mutex);

		T t = _dataQueue.front();
		_dataQueue.pop();
		return t;
	}

	void push(const T& t) {
		std::lock_guard<std::mutex> lock(_mutex);

		_dataQueue.push(t);

		if (_coroQueue.size()) {
			_coroQueue.front()();
			_coroQueue.pop();
		} else {
			++_size;
		}
	}


private:
	std::mutex _mutex;
	size_t _size = 0; //< кол-во элементов, которые ещё никому не нужны
	std::queue<T> _dataQueue;
	std::queue<std::function<void()>> _coroQueue;
};
