
#include <boost/test/unit_test.hpp>


BOOST_AUTO_TEST_SUITE(SuiteExperiments)


BOOST_AUTO_TEST_CASE(Test) {
	uint32_t a = 1;
	uint32_t b = 2;
	try {
		try {
			throw a;
		}
		catch (...) {
			try {
				throw b;
			}
			catch (...) {
				throw;
			}
		}
	}
	catch (uint32_t result) {
		BOOST_REQUIRE(result == b);
	}
}


BOOST_AUTO_TEST_CASE(Test2) {
	uint32_t a = 1;
	uint32_t b = 2;
	std::exception_ptr exception;
	try {
		throw a;
	}
	catch (...) {
		try {
			throw b;
		}
		catch (...) {
		}
		exception = std::current_exception();
	}
	try {
		std::rethrow_exception(exception);
	}
	catch (uint32_t result) {
		BOOST_REQUIRE(result == a);
	}
}


#ifndef _MSC_VER // бага VC++, не планируют исправлять

class A {
public:
	~A()
	{
		try
		{
			throw std::runtime_error("oh no");
		}
		catch (std::exception &)
		{
			BOOST_REQUIRE(std::current_exception());
		}
	}
};


BOOST_AUTO_TEST_CASE(Test3) {
	A a;
}


BOOST_AUTO_TEST_CASE(Test4) {
	try {
		A a;
		throw std::runtime_error("oh no");
	}
	catch(...) {
	}
}

#endif



BOOST_AUTO_TEST_SUITE_END()
