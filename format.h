
#pragma once

#include <boost/format.hpp>

inline std::string format(boost::format& fmt) {
	return fmt.str();
}

template <typename First, typename... Rest>
std::string format(boost::format fmt, const First& first, Rest... rest) {
	return format(fmt % first, rest...);
}

template <typename... Params>
std::string format(const std::string& msg, Params... params) {
	return format(boost::format(msg), params...);
}
