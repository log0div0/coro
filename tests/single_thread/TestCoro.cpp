
#include <boost/test/unit_test.hpp>
#include "Coro.h"


BOOST_AUTO_TEST_SUITE(SuiteCoro)


BOOST_AUTO_TEST_CASE(Test) {
	bool success = false;
	Coro coro([&] {
		success = true;
	});
	coro.resume();
	BOOST_REQUIRE(success);
}


BOOST_AUTO_TEST_CASE(TestLeonardoDicaprio) {
	bool success = false;
	Coro coro1([&] {
		Coro coro2([&] {
			success = true;
		});
		auto current = Coro::current();
		coro2.resume();
		BOOST_REQUIRE(current == Coro::current());
	});
	coro1.resume();
	BOOST_REQUIRE(success);
}

BOOST_AUTO_TEST_CASE(TestCancel) {
	bool success = false;
	Coro coro([&] {
		try {
			Coro::current()->yield();
		}
		catch (const CancelError& error) {
			success = true;
		}
	});
	coro.resume();
	coro.cancel();
	BOOST_REQUIRE(success);
}


BOOST_AUTO_TEST_SUITE_END()
