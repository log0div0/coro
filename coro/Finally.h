
#pragma once

#include <functional>

class Finally {
public:
	Finally(std::function<void()> fn): _fn(std::move(fn)) {}
	~Finally() {
		_fn();
	}

private:
	std::function<void()> _fn;
};
