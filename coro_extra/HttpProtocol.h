#pragma once

#include "coro_extra/Buffer.h"
#include "coro_extra/HttpParsers.h"
#include <boost/algorithm/string.hpp>

class HttpProtocol {
public:
	template <typename Iterator>
	struct HttpChunkedHead: boost::spirit::qi::grammar<Iterator, int()>
	{
		HttpChunkedHead(): HttpChunkedHead::base_type(_len)
		{
			using namespace boost::spirit::qi;

			_len = hex >> omit[lit("\r\n")];
		}

		boost::spirit::qi::rule<Iterator, int()> _len;
	};


	void writeGET(const std::string& url, const HttpHeaders& headers, Buffer& buffer) {
		HttpUrl parsedUrl;
		HttpUrlParser<decltype(url.begin())> parser;
		if (!boost::spirit::qi::parse(url.begin(), url.end(), parser, parsedUrl)) {
			throw std::runtime_error("Invalid HTTP url");
		};

		buffer.pushBack("GET ");
		buffer.pushBack(parsedUrl.get_path());
		buffer.pushBack(" HTTP/1.1\r\n");

		buffer.pushBack("Host: ");
		buffer.pushBack(parsedUrl.host);
		buffer.pushBack("\r\n");

		for(const auto& header : headers) {
			buffer.pushBack(header.first);
			buffer.pushBack(": ");
			buffer.pushBack(header.second);
			buffer.pushBack("\r\n");
		}

		buffer.pushBack("\r\n");
	};

	template<typename Iterator>
	Buffer::Iterator readResponse(const Iterator& begin, const Iterator& end, Buffer& body) {
		Iterator it = begin;
		HttpResponseParser<Iterator> parser;
		if (!boost::spirit::qi::parse(it, end, parser, _response)) {
			throw std::runtime_error("Invalid HTTP response");
		};

		bool chunked = false;
		int contentLength = -1;

		for (const auto& header : _response.headers) {
			if (boost::iequals(std::string("Transfer-Encoding"), header.first)) {
				if (boost::iequals(std::string("chunked"), header.second)) {
					chunked = true;
				}
			} else if (boost::iequals(std::string("Content-Length"), header.first)) {
				contentLength = atoi(header.second.c_str());
			}
		}

		if (chunked) {
			HttpChunkedHead<Iterator> partParser;
			while (true) {
				int partSize;
				if (!boost::spirit::qi::parse(it, end, partParser, partSize)) {
					throw std::runtime_error("Invalid chunked part");
				}
				if (partSize == 0) {
					it += 2; // skip last "\r\n"
					break;
				}
				*(it+partSize);
				Buffer::Iterator b = it;
				Buffer::Iterator e = it+partSize;
				body.pushBack(b, e);
				it += partSize+2;// skip ending "\r\n"
			}
		} else {
			if (contentLength > 0) {
				*(it+contentLength);
				Buffer::Iterator b = it;
				Buffer::Iterator e = it+contentLength;
				body.pushBack(b, e);
				it += contentLength;
			} else if (contentLength == -1) {
				Iterator bodyEnd = it;
				while (true) {
					bool isAll = false;
					try {
						isAll = bodyEnd+1 == end;
					} catch(...) {
						isAll = true;
					}
					if (isAll) {
						*bodyEnd;
						Buffer::Iterator b = it;
						Buffer::Iterator e = bodyEnd;
						body.pushBack(b, e);
						it = bodyEnd;
						break;
					} else {
						++bodyEnd;
					}
				}
			}
		}

		return it;
	};

	const HttpResponse& response() const {
		return _response;
	}

private:
	HttpResponse _response;
};
