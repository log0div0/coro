#include <atomic>
#include <catch.hpp>
#include "coro/StrandPool.h"

using namespace coro;

TEST_CASE("StrandPool::cancelAll", "[StrandPool]") {
	StrandPool pool;

	pool.exec([] {
		Coro::current()->yield({TokenThrow});
	});
	pool.exec([] {
		Coro::current()->yield({TokenThrow});
	});

	pool.cancelAll();
	REQUIRE_NOTHROW(pool.waitAll(false));
}

TEST_CASE("StrandPool stress test", "[StrandPool]") {
	StrandPool pool;

	for (int i = 0; i < 20; i++) {
		std::shared_ptr<Strand> strand = pool.exec([] {
			for(int i=0; i < 10000; i++) {
				Strand::current()->post([coro = Coro::current()]() { coro->resume("test"); });
				Coro::current()->yield({"test"});
			}
		});
	}

	REQUIRE_NOTHROW(pool.waitAll(false));
}

TEST_CASE("Cyclical StrandPools posting", "[StrandPool]") {
	StrandPool firstPool, secondPool;
	uint64_t firstCounter = 0, secondCounter = 0;
	std::shared_ptr<Strand> firstStrand, secondStrand;
	Coro *firstCoro, *secondCoro;

	firstStrand = firstPool.exec([&] {
		firstCoro = Coro::current();

		firstCoro->yield({"first"});

		while (firstCounter < 10000) {
			secondStrand->post([&] {
				secondCoro->resume("second");
			});
			firstCounter++;
			firstCoro->yield({"first"});
		}
	});

	secondStrand = secondPool.exec([&] {
		secondCoro = Coro::current();

		firstStrand->post([&] {
			firstCoro->resume("first");
		});

		while (secondCounter < 10000) {
			secondCounter++;
			secondCoro->yield({"second"});
			firstStrand->post([&] {
				firstCoro->resume("first");
			});
		}
	});

	REQUIRE_NOTHROW(firstPool.waitAll(false));
	REQUIRE_NOTHROW(secondPool.waitAll(false));
}