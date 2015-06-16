
#pragma once

#include "coro/Coro.h"
#include "coro/Finally.h"
#include <queue>
#include <list>

template <typename T>
class Queue {
public:
	T pop() {
		if (_data.empty()) {
			Finally cleanup([&] {
				_coros.remove(Coro::current());
			});
			_coros.push_back(Coro::current());
			_coros.back()->yield();
		}

		T t = std::move(_data.front());
		_data.pop();
		return t;
	}

	template <typename U>
	void push(U&& u) {
		_data.push(std::forward<U>(u));

		if (!_coros.empty()) {
			_coros.front()->resume();
		}
	}

	size_t size() const {
		return _data.size();
	}

private:
	std::queue<T> _data;
	std::list<Coro*> _coros;
};
