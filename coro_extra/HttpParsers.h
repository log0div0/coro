
#pragma once


#include <map>
#include <boost/spirit/include/qi.hpp>
#include <boost/fusion/include/std_pair.hpp>
#include <boost/fusion/include/adapt_struct.hpp>


typedef std::map<std::string, std::string> HttpHeaders;

struct HttpRequest {
	HttpHeaders headers;
};

struct HttpResponse {
	int code;
	HttpHeaders headers;
};


BOOST_FUSION_ADAPT_STRUCT(HttpRequest, (HttpHeaders, headers));
BOOST_FUSION_ADAPT_STRUCT(HttpResponse, (int, code) (HttpHeaders, headers));


template <typename Iterator>
struct HttpHeadersParser: boost::spirit::qi::grammar<Iterator, HttpHeaders()>
{
	HttpHeadersParser(): HttpHeadersParser::base_type(_allHeaders)
	{
		using namespace boost::spirit::qi;

		_allHeaders = +(_header >> "\r\n");
		_header = _key >> ": " >> (_certificate | _value);
		_key = char_("a-zA-Z_") >> *char_("a-zA-Z_0-9-");
		_certificate = raw[
			"-----BEGIN CERTIFICATE-----\r\n" >>
			+char_("a-zA-Z_0-9/=+\r\n") >>
			"-----END CERTIFICATE-----"
		];
		_value = +(char_ - '\r');
	}

	boost::spirit::qi::rule<Iterator, HttpHeaders()> _allHeaders;
	boost::spirit::qi::rule<Iterator,
		std::pair<std::string, std::string>()> _header;
	boost::spirit::qi::rule<Iterator, std::string()> _key, _value, _certificate;
};


template <typename Iterator>
struct HttpRequestParser: boost::spirit::qi::grammar<Iterator, HttpRequest()>
{
	HttpRequestParser(): HttpRequestParser::base_type(_request)
	{
		using namespace boost::spirit::qi;

		_request = "GET "
			>> _url
			>> " " >> (lit("HTTP/1.0") | lit("HTTP/1.1")) >> "\r\n"
			>> _headers >> "\r\n";
		_url = +(char_ - ' ');
	}

	boost::spirit::qi::rule<Iterator, HttpRequest()> _request;
	boost::spirit::qi::rule<Iterator> _url;
	HttpHeadersParser<Iterator> _headers;
};


template <typename Iterator>
struct HttpResponseParser: boost::spirit::qi::grammar<Iterator, HttpResponse()>
{
	HttpResponseParser(): HttpResponseParser::base_type(_response)
	{
		using namespace boost::spirit::qi;

		_response = (lit("HTTP/1.0") | lit("HTTP/1.1")) >> " "
			>> int_
			>> omit[+(char_ - "\r")] >> "\r\n"
			>> _headers >> "\r\n";
	}

	boost::spirit::qi::rule<Iterator, HttpResponse()> _response;
	HttpHeadersParser<Iterator> _headers;
};
