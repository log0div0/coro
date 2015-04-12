
#pragma once

#include "Task.h"
#include <mutex>
#include <queue>

template <typename T>
class CoroQueue {
public:
	T pop() {
		while (true) {
			auto task = std::make_shared<Task>();
			{
				std::unique_lock<std::mutex> lock(_mutex);

				if (_dataQueue.size()) {
					T t = std::move(_dataQueue.front());
					_dataQueue.pop();
					return t;
				}

				_taskQueue.push(task);
			}
			try {
				Coro::current()->yield();
			}
			catch (...) {
				task->cancel();

				// следующий ...

				std::unique_lock<std::mutex> lock(_mutex);

				if (_taskQueue.size()) {
					(*_taskQueue.front())();
					_taskQueue.pop();
				}

				throw;
			}
		}
	}

	template <typename U>
	void push(U&& u) {
		std::unique_lock<std::mutex> lock(_mutex);

		_dataQueue.push(std::forward<U>(u));

		if (_taskQueue.size()) {
			(*_taskQueue.front())();
			_taskQueue.pop();
		}
	}


private:
	std::mutex _mutex;
	std::queue<T> _dataQueue;
	std::queue<std::shared_ptr<Task>> _taskQueue;
};
