#pragma once

#include <boost/fusion/include/adapt_struct.hpp>

#include "coro_extra/Buffer.h"
#include "coro_extra/HttpParsers.h"

enum class FcgiRequestType: uint8_t {
	BeginRequest = 1,
	AbortRequest = 2,
	EndRequest = 3,
	Params = 4,
	StdIn = 5,
	StdOut = 6,
	StdErr = 7,
	Data = 8,
	GetValues = 9,
	GetValuesResult = 10,
	UnknownType = 11
};

#pragma pack(push,1)
struct FcgiHeader {
	FcgiHeader(FcgiRequestType t, size_t size);

	uint8_t version;
	uint8_t type;
	uint8_t requestIdB1;
	uint8_t requestIdB0;
	uint8_t contentLengthB1;
	uint8_t contentLengthB0;
	uint8_t paddingLength;
	uint8_t reserved;

	uint8_t* begin() {
		return (uint8_t*)this;
	}

	uint8_t* end() {
		return ((uint8_t*)this) + sizeof(*this);
	}
};

struct FcgiBeginRequestBody {
	uint8_t roleB1;
	uint8_t roleB0;
	uint8_t flags;
	uint8_t reserved[5];

	uint8_t* begin() {
		return (uint8_t*)this;
	}

	uint8_t* end() {
		return ((uint8_t*)this) + sizeof(*this);
	}
};
#pragma pack(pop)

struct FcgiResponse
{
	int status;
	HttpHeaders headers;
	std::string body;
};

BOOST_FUSION_ADAPT_STRUCT(FcgiResponse, (int, status) (HttpHeaders, headers) (std::string, body));

template <typename Iterator>
struct FcgiResponseParser: boost::spirit::qi::grammar<Iterator, FcgiResponse()>
{
	FcgiResponseParser(): FcgiResponseParser::base_type(_response)
	{
		using namespace boost::spirit::qi;

		_response = lit("Status: ") >> int_ >> omit[+(char_ - "\r")] >> "\r\n"
			>> _headers >> "\r\n"
			>> *char_;
	}

	boost::spirit::qi::rule<Iterator, FcgiResponse()> _response;
	HttpHeadersParser<Iterator> _headers;
};


class FcgiClient {
public:
	void writeRequest(const std::string& method, const std::string& URI, const std::string& body,
		const std::string server, uint16_t port, Buffer& buffer);

	template <typename Iterator>
	FcgiResponse readResponse(const Iterator& begin, const Iterator& end) {
		Iterator it = begin;

		std::string resp("");
		while (1)
		{
			FcgiHeader responseHeader(FcgiRequestType::UnknownType, 0);
			std::copy(it, it+sizeof(FcgiHeader), responseHeader.begin());
			it += sizeof(FcgiHeader);
			if (responseHeader.type == (uint8_t)FcgiRequestType::EndRequest) {
				break;
			}

			size_t len = responseHeader.contentLengthB1;
			len = len << 8;
			len += responseHeader.contentLengthB0;
			if (len > 0) {
				std::string text_response;
				text_response.resize(len);
				std::copy(it, it+len, text_response.begin());
				it += len;
				resp += text_response;
			}

			if (responseHeader.paddingLength > 0) {
				std::vector<uint8_t> padding;
				padding.resize(responseHeader.paddingLength);
				std::copy(it, it+padding.size(), padding.begin());
				it += padding.size();
			}
		}

		printf("%s\n", resp.data());
		FcgiResponseParser<std::string::iterator> parser;
		FcgiResponse result;
		auto respit = resp.begin();
		if (!boost::spirit::qi::parse(respit, resp.end(), parser, result)) {
			throw std::runtime_error("Fcgi client: bad http response");
		}
		return result;
	}
};
