
#include <boost/test/unit_test.hpp>
#include "Timeout.h"
#include <chrono>


using namespace std::chrono_literals;


BOOST_AUTO_TEST_SUITE(SuiteTimeout)


BOOST_AUTO_TEST_CASE(TestCoroIsInactive) {
	auto coro = Coro::current();

	Timeout timeout(100ms);

	BOOST_REQUIRE_THROW(coro->yield(), std::exception);
}


BOOST_AUTO_TEST_CASE(TestCoroIsActive) {
	auto coro = Coro::current();

	Timeout timeout(100ms);

	std::this_thread::sleep_for(200ms);

	BOOST_REQUIRE_THROW(coro->yield(), std::exception);
}


BOOST_AUTO_TEST_CASE(TestCoroIsScheduled) {
	auto coro = Coro::current();

	Timeout timeout(100ms);

	std::this_thread::sleep_for(200ms);

	coro->schedule();
	BOOST_REQUIRE_THROW(coro->yield(), std::exception);
}


BOOST_AUTO_TEST_CASE(TestCancel) {
	auto coro = Coro::current();

	{
		Timeout timeout(100ms);
	}

	std::this_thread::sleep_for(200ms);

	coro->schedule();
	BOOST_REQUIRE_NO_THROW(coro->yield());
}


// BOOST_AUTO_TEST_CASE(TestCancel2) {
// 	auto coro = Coro::current();

// 	{
// 		Timeout timeout(100ms);

// 		std::this_thread::sleep_for(200ms);
// 	}

// 	// Таймфут сработал, но он нам уже не нужен

// 	coro->schedule();
// 	BOOST_REQUIRE_NO_THROW(coro->yield());
// }


// BOOST_AUTO_TEST_CASE(TestTwoTimeouts) {
// 	auto coro = Coro::current();

// 	Timeout timeout(100ms);
// 	Timeout timeout2(100ms);
// 	BOOST_REQUIRE_THROW(coro->yield(), std::exception);
// 	BOOST_REQUIRE_THROW(coro->yield(), std::exception);
// }


BOOST_AUTO_TEST_SUITE_END()
