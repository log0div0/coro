
#pragma once

#include <boost/lockfree/queue.hpp>

template <typename T>
class ObjectPool {
public:
	ObjectPool(): _queue(1024) {
	}

	~ObjectPool() {
		T* t;
		while (_queue.pop(t)) {
			delete t;
		}
	}

	T* pop() {
		T* t;
		if (_queue.pop(t)) {
			return t;
		} else {
			return new T;
		}
	}

	void push(T* t) {
		_queue.push(t);
	}

	static std::unique_ptr<T, std::function<void(T*)>&> take() {
		static ObjectPool<T> pool;
		static std::function<void(T*)> pushObjectBack = [&](T* t) {
			pool.push(t);
		};
		return { pool.pop(), pushObjectBack };
	}

private:
	boost::lockfree::queue<T*, boost::lockfree::fixed_sized<false>> _queue;
};
