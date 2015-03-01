
#pragma once

#include "ThreadPool.h"
#include "Coro.h"
#include <mutex>
#include <queue>

template <typename T>
class CoroQueue {
public:
	T pop() {
		_mutex.lock();

		if (_size) {
			--_size;
			T t = _dataQueue.front();
			_dataQueue.pop();
			_mutex.unlock();
			return t;
		}

		_coroQueue.push([threadPool = ThreadPool::current(), coro = Coro::current()] {
			threadPool->schedule([=]() {
				coro->resume();
			});
		});

		Coro::current()->yield([&]() {
			_mutex.unlock();
		});

		std::lock_guard<std::mutex> lock(_mutex);

		T t = std::move(_dataQueue.front());
		_dataQueue.pop();
		return t;
	}

	template <typename U>
	void push(U&& u) {
		std::lock_guard<std::mutex> lock(_mutex);

		_dataQueue.push(std::forward<U>(u));

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
