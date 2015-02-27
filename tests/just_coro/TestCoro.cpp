
#include <boost/test/unit_test.hpp>
#include "Coro.h"


BOOST_AUTO_TEST_SUITE(SuiteCoro)


BOOST_AUTO_TEST_CASE(TestResumeYield) {
	std::vector<uint32_t> a {1, 2, 3, 4, 5}, b;

	Coro coro([&] {
		b.push_back(2);
		Yield();
		b.push_back(4);
	});
	b.push_back(1);
	coro.resume();
	b.push_back(3);
	coro.resume();
	b.push_back(5);

	BOOST_REQUIRE(a == b);
}


BOOST_AUTO_TEST_CASE(TestYieldWithCallback) {
	Coro coro([&] {
		Yield([]() {
			BOOST_REQUIRE(!Coro::current());
		});
	});

	coro.resume();
}


BOOST_AUTO_TEST_CASE(TestException) {
	Coro coro([&] {
		throw std::runtime_error("catch me");
	});

	coro.resume();

	BOOST_REQUIRE(coro.exception() != std::exception_ptr());

	try {
		std::rethrow_exception(coro.exception());
	} catch (const std::exception& exception) {
		BOOST_REQUIRE(exception.what() == std::string("catch me"));
	}
}


BOOST_AUTO_TEST_SUITE_END()
