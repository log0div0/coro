
#pragma once

#include "Stream.h"

class UnixSocket: public Stream<boost::asio::local::stream_protocol::socket> {
public:
	UnixSocket();
	UnixSocket(boost::asio::local::stream_protocol::socket socket);

	UnixSocket(UnixSocket&& other);
	UnixSocket& operator=(UnixSocket&& other);

	void connect(const std::string& endpoint);
};
