
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
			std::unique_lock<std::mutex> lock(_mutex);

			if (_size) {
				--_size;
				T t = std::move(_dataQueue.front());
				_dataQueue.pop();
				return t;
			}

			_coroQueue.push([threadPool = ThreadPool::current(), coro = Coro::current()] {
				threadPool->schedule([=]() {
					coro->resume();
				});
			});

			Coro::current()->yield([&]() {
				lock.unlock();
			});
		}

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

		std::function<void()> callback;
		if (_coroQueue.size()) {
			callback = std::move(_coroQueue.front());
			_coroQueue.pop();
		} else {
			++_size;
		}

		lock.unlock();

		if (callback) {
			callback();
		}
	}


private:
	std::mutex _mutex;
	size_t _size = 0; //< кол-во элементов, которые ещё никому не нужны
	std::queue<T> _dataQueue;
	std::queue<std::function<void()>> _coroQueue;
};
