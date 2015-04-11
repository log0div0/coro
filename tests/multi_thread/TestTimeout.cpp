
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


BOOST_AUTO_TEST_SUITE_END()
