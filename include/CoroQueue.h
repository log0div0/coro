
#pragma once

#include "Coro.h"
#include <mutex>
#include <queue>

template <typename T>
class CoroQueue {
public:
	T pop() {
		while (true) {
			auto task = std::make_shared<CoroTask>();
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
				std::unique_lock<std::mutex> lock(_mutex);
				task->preventExecution();
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
		while (_taskQueue.size()) {
			auto task = std::move(_taskQueue.front());
			_taskQueue.pop();
			if (task->schedule()) {
				return;
			}
		}
	}

private:
	std::mutex _mutex;
	std::queue<T> _dataQueue;
	std::queue<std::shared_ptr<CoroTask>> _taskQueue;
};
