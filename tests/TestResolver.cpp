
#include <boost/test/unit_test.hpp>
#include "Resolver.h"


using namespace boost::asio::ip;


BOOST_AUTO_TEST_SUITE(SuiteResolver)


BOOST_AUTO_TEST_CASE(TestSuccess) {
	TcpResolver resolver;
	auto it = resolver.resolve(tcp::resolver::query(tcp::v4(), "localhost", "12345"));
	BOOST_REQUIRE(*it == tcp::endpoint(address_v4::from_string("127.0.0.1"), 12345));
}


BOOST_AUTO_TEST_CASE(TestFailure) {
	TcpResolver resolver;
	BOOST_REQUIRE_THROW(resolver.resolve(tcp::resolver::query(tcp::v4(), "@", "12345")), std::runtime_error);
}


BOOST_AUTO_TEST_SUITE_END()
