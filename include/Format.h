
#pragma once

#include <boost/format.hpp>

template <typename Char>
std::basic_string<Char> Format(boost::basic_format<Char>& fmt) {
	return fmt.str();
}

template <typename Char, typename First, typename... Rest>
std::basic_string<Char> Format(boost::basic_format<Char> fmt, const First& first, Rest... rest) {
	return Format(fmt % first, rest...);
}

template <typename Char, typename... Params>
std::basic_string<Char> Format(const std::basic_string<Char>& msg, Params... params) {
	return Format(boost::basic_format<Char>(msg), params...);
}

template <typename Char, typename... Params>
std::basic_string<Char> Format(const Char msg[], Params... params) {
	return Format(boost::basic_format<Char>(msg), params...);
}
