
#pragma once

#include <asio/steady_timer.hpp>
#include "Coro/AsioTask.h"
#include "Coro/IoService.h"

namespace coro {

/// Wrapper вокруг asio::steady_timer
class Timer {
public:
	Timer(): _handle(*IoService::current()) {}

	template <typename Duration>
	void waitFor(Duration duration) {
		_handle.expires_from_now(duration);
		wait();
	}

	template <typename Timestamp>
	void waitUntil(Timestamp timestamp) {
		_handle.expires_at(timestamp);
		wait();
	}

private:
	void wait() {
		AsioTask1 task;
		_handle.async_wait(task.callback());
		task.wait(_handle);
	}

	asio::steady_timer _handle;
};

}