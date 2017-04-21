#pragma once

#include "coro_extra/TcpSocket.h"

namespace coro {

namespace Fcgi {

enum class RequestType: uint8_t {
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
struct Header {
	Header(RequestType t, size_t size);

	uint8_t version;
	uint8_t type;
	uint8_t requestIdB1;
	uint8_t requestIdB0;
	uint8_t contentLengthB1;
	uint8_t contentLengthB0;
	uint8_t paddingLength;
	uint8_t reserved;

	asio::const_buffer as_buf() const {
		return asio::const_buffer(this, sizeof(*this));
	}

	asio::mutable_buffer as_mbuf() {
		return asio::mutable_buffer(this, sizeof(*this));
	}
};

struct BeginRequestBody {
	uint8_t roleB1;
	uint8_t roleB0;
	uint8_t flags;
	uint8_t reserved[5];

	asio::const_buffer as_buf() const {
		return asio::const_buffer(this, sizeof(*this));
	}
};
#pragma pack(pop)

struct Response
{
	std::string text;
	std::string headers;
	int status;
};

class Client {
public:
	static Response make_request(const std::string& method, const std::string& URI, const std::string& body,
		TcpSocket& socket, const std::string server, uint16_t port);
};

}
}
