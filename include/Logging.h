
#pragma once

#include <boost/log/trivial.hpp>

#define BOOST_LOG_TRACE BOOST_LOG_TRIVIAL(trace) << "[" << __FILE__ << "::" << __LINE__ << "::" << __FUNCTION__ << "] "
