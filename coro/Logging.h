
#pragma once

#include <boost/log/trivial.hpp>

#ifdef _MSC_VER
#define __PRETTY_FUNCTION__ __FUNCDNAME__
#endif

#define BOOST_LOG_TRACE BOOST_LOG_TRIVIAL(trace) << __PRETTY_FUNCTION__ << ":" << __LINE__ << ": "
