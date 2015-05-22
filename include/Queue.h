
#pragma once

#include "Coro.h"
#include <queue>
#include <list>

template <typename T>
class Queue {
public:
	T pop() {
		if (_data.empty()) {
			try {
				_consumers.push_back(Coro::current());
				_consumers.back()->yield();
			}
			catch (...) {
				_consumers.remove(Coro::current());
				throw;
			}
		}

		T t = std::move(_data.front());
		_data.pop();
		return t;
	}

	template <typename U>
	void push(U&& u) {
		_data.push(std::forward<U>(u));

		if (!_consumers.empty()) {
			_consumers.front()->resume();
			_consumers.pop_front();
		}
	}

	size_t size() const {
		return _data.size();
	}

private:
	std::queue<T> _data;
	std::list<Coro*> _consumers;
};
