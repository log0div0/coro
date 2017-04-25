
#include "base64.h"
#include <boost/algorithm/string/classification.hpp>
#include <boost/algorithm/string/trim.hpp>
#include <boost/archive/iterators/base64_from_binary.hpp>
#include <boost/archive/iterators/binary_from_base64.hpp>
#include <boost/archive/iterators/insert_linebreaks.hpp>
#include <boost/archive/iterators/transform_width.hpp>

using namespace std;
using namespace boost;
using namespace boost::archive::iterators;

string base64::encode(const void *data, size_t size)
{
	const uint8_t *d = reinterpret_cast<const uint8_t *>(data);
	vector<uint8_t> vd(d, d + size);
	return encode(vd);
}

string base64::encode(const vector<uint8_t> &data)
{
	string result;
	typedef base64_from_binary<transform_width<vector<uint8_t>::const_iterator, 6, 8> > base64_enc;
	copy(base64_enc(data.begin()), base64_enc(data.end()), back_inserter(result));

	string tails[3] = {"", "==", "="};
	return result + tails[data.size() % 3];
}

size_t base64::decode(const string &str, void *data, size_t size)
{
	const auto vd = decode(str);
	if (data != nullptr) {
		uint8_t *d = reinterpret_cast<uint8_t *>(data);
		copy(vd.begin(), vd.begin() + min(vd.size(), size), d);
	}
	return vd.size();
}

vector<uint8_t> base64::decode(const string &str)
{
	string encoded = str;
	encoded.erase(std::remove_if(encoded.begin(), encoded.end(),
				[](char x) {
					return isspace(x);
				}), encoded.end());
	trim_right_if(encoded, is_any_of("="));

	vector<uint8_t> data;
	data.reserve(encoded.size() * 3 / 4 + 3);	// Если даже ошибусь - вектор доресайзится сам.

	typedef transform_width<binary_from_base64<string::const_iterator>, 8, 6> base64_dec;
	copy(base64_dec(encoded.begin()), base64_dec(encoded.end()), back_inserter(data));
	data.shrink_to_fit();
	return data;
}

string base64::split(const string &str)
{
	string result;
	typedef insert_linebreaks<string::const_iterator, 64> linebreak;
	copy(linebreak(str.begin()), linebreak(str.end()), back_inserter(result));
	return result;
}

// Функции для поддержания старого кода
std::string base64_encode(unsigned char const* bytes_to_encode, unsigned int in_len)
{
	return base64::encode(bytes_to_encode, in_len);
}

std::string base64_decode(std::string const& encoded_string)
{
	const auto data = base64::decode(encoded_string);
	return string(data.begin(), data.end());
}

std::string base64_encode_ex(const std::vector<uint8_t>& data)
{
	return base64::encode(data);
}

std::vector<uint8_t> base64_decode_ex(const std::string& base64_data)
{
	return base64::decode(base64_data);
}

