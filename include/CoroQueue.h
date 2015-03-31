
#pragma once

#include "Task.h"
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

			_taskQueue.push(Task::current());

			CoroYield([&]() {
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

		Task task;
		if (_taskQueue.size()) {
			task = std::move(_taskQueue.front());
			_taskQueue.pop();
		} else {
			++_size;
		}

		lock.unlock();

		task();
	}


private:
	std::mutex _mutex;
	size_t _size = 0; //< кол-во элементов, которые ещё никому не нужны
	std::queue<T> _dataQueue;
	std::queue<Task> _taskQueue;
};
