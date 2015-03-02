
#include <boost/test/unit_test.hpp>
#include "BlockingBufferPool.h"
#include "NonBlockingBufferPool.h"
#include "CoroPool.h"


BOOST_AUTO_TEST_SUITE(SuiteBufferPool)


BOOST_AUTO_TEST_CASE(TestBlockingBufferPool) {
	std::vector<uint8_t> actual, expected = {1, 2, 3, 4};

	BlockingBufferPool pool(2, 1000);

	BOOST_REQUIRE(pool.size() == 2);

	auto a = pool.makeUnique();
	auto b = pool.makeShared();

	actual.push_back(1);

	Exec([&, copy = std::move(b)]() {
		actual.push_back(3);
	});

	actual.push_back(2);
	auto c = pool.makeUnique();
	actual.push_back(4);

	BOOST_REQUIRE(pool.size() == 2);
	BOOST_REQUIRE(actual == expected);
}


BOOST_AUTO_TEST_CASE(TestNonBlockingBufferPool) {
	NonBlockingBufferPool pool(1000);

	BOOST_REQUIRE(pool.size() == 0);

	auto a = pool.makeUnique();
	auto b = pool.makeShared();

	BOOST_REQUIRE(pool.size() == 2);

	a.reset();
	b.reset();

	BOOST_REQUIRE(pool.size() == 2);
}


BOOST_AUTO_TEST_SUITE_END()
