
#pragma once


#include <boost/archive/iterators/transform_width.hpp>
#include <boost/archive/iterators/base64_from_binary.hpp>
#include <boost/archive/iterators/binary_from_base64.hpp>


namespace base64 {

template <typename Iterator>
std::string encode(Iterator begin, Iterator end) {
	typedef boost::archive::iterators::base64_from_binary <
		boost::archive::iterators::transform_width<const char*, 6, 8>
	> base64_from_binary;

	std::string result;

	std::stringstream stream;
	std::copy(base64_from_binary(begin), base64_from_binary(end),
		std::ostream_iterator<char>(stream));
	result = stream.str();

	return result + std::string(4 - result.size() % 4, '=');
}

template <typename T>
std::string encode(const T& t) {
	return encode(t.data(), t.data() + t.size());
}

template <typename Iterator>
void decode(const std::string& base64_string, Iterator output) {
	typedef boost::archive::iterators::transform_width<
		boost::archive::iterators::binary_from_base64<const char *>, 8, 6
	> base64_to_binary;

	unsigned int size = base64_string.size();

	// Remove the padding characters, cf. https://svn.boost.org/trac/boost/ticket/5629
	if (size && base64_string[size - 1] == '=') {
		--size;
		if (size && base64_string[size - 1] == '=') {
			--size;
		}
	}

	if (size == 0) {
		return;
	}

	std::copy(
		base64_to_binary(base64_string.data()),
		base64_to_binary(base64_string.data() + size),
		output);
}

}