
#include <boost/test/unit_test.hpp>
#include "Timeout.h"
#include "CoroPool.h"
#include "CoroQueue.h"
#include "CoroMutex.h"
#include "TcpServer.h"
#include "TcpSocket.h"
#include <chrono>


using namespace std::chrono_literals;
using namespace boost::asio::ip;

static auto endpoint = tcp::endpoint(address_v4::from_string("127.0.0.1"), 44442);
static Buffer test_data { 0x01, 0x02, 0x03, 0x04 };


BOOST_AUTO_TEST_SUITE(SuiteTimeout)


BOOST_AUTO_TEST_CASE(TestCoroIsInactive) {
	auto coro = Coro::current();

	Timeout timeout(100ms);

	BOOST_REQUIRE_THROW(coro->yield(), TimeoutError);
}


BOOST_AUTO_TEST_CASE(TestCoroIsActive) {
	auto coro = Coro::current();

	Timeout timeout(100ms);

	std::this_thread::sleep_for(200ms);

	BOOST_REQUIRE_THROW(coro->yield(), TimeoutError);
}


BOOST_AUTO_TEST_CASE(TestCoroIsScheduled) {
	Exec([] {
		auto coro = Coro::current();

		Timeout timeout(100ms);

		std::this_thread::sleep_for(200ms);

		auto task = std::make_shared<Task>();
		(*task)();
		try {
			coro->yield();
		}
		catch (const TimeoutError&) {
			task->cancel();
		}
	});
	Join();
}


BOOST_AUTO_TEST_CASE(TestCancel) {
	auto coro = Coro::current();

	{
		Timeout timeout(100ms);
	}

	std::this_thread::sleep_for(200ms);

	coro->strand()->post([&] {
		coro->resume();
	});
	BOOST_REQUIRE_NO_THROW(coro->yield());
}


BOOST_AUTO_TEST_CASE(TestCancel2) {
	auto coro = Coro::current();

	{
		Timeout timeout(100ms);

		std::this_thread::sleep_for(200ms);
	}

	// Таймфут сработал, но он нам уже не нужен

	coro->strand()->post([&] {
		coro->resume();
	});
	BOOST_REQUIRE_NO_THROW(coro->yield());
}


BOOST_AUTO_TEST_CASE(TestTwoTimeouts) {
	auto coro = Coro::current();

	Timeout timeout(100ms);
	Timeout timeout2(100ms);
	BOOST_REQUIRE_THROW(coro->yield(), TimeoutError);
	BOOST_REQUIRE_THROW(coro->yield(), TimeoutError);
}


BOOST_AUTO_TEST_CASE(TestCoroQueue) {
	Timeout timeout(100ms);
	CoroQueue<uint64_t> queue;
	BOOST_REQUIRE_THROW(queue.pop(), TimeoutError);
	queue.push(0);
}


BOOST_AUTO_TEST_CASE(TestCoroMutex) {
	Timeout timeout(100ms);
	CoroMutex mutex;
	std::lock_guard<CoroMutex> lock(mutex);
	BOOST_REQUIRE_THROW(mutex.lock(), TimeoutError);
}


BOOST_AUTO_TEST_CASE(TestCoroPool) {
	CoroPool pool;
	uint64_t i = 0;
	pool.exec([&] {
		std::this_thread::sleep_for(200ms);
		i = 10;
	});
	Timeout timeout(100ms);
	BOOST_REQUIRE_THROW(pool.join(), TimeoutError);
	BOOST_REQUIRE(i == 0);
	BOOST_REQUIRE_NO_THROW(pool.join());
	BOOST_REQUIRE(i == 10);
}


BOOST_AUTO_TEST_CASE(TestTcpServerAccept) {
	Timeout timeout(100ms);
	TcpServer server(endpoint);
	BOOST_REQUIRE_THROW(server.accept(), TimeoutError);
}


BOOST_AUTO_TEST_SUITE_END()
