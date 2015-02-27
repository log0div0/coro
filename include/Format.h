
#pragma once

#include <boost/format.hpp>

inline std::string Format(boost::format& fmt) {
	return fmt.str();
}

template <typename First, typename... Rest>
std::string Format(boost::format fmt, const First& first, Rest... rest) {
	return Format(fmt % first, rest...);
}

template <typename... Params>
std::string Format(const std::string& msg, Params... params) {
	return Format(boost::format(msg), params...);
}
