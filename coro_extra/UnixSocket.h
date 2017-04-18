
#pragma once

#include "coro/StreamSocket.h"

namespace coro {

class UnixSocket: public StreamSocket<asio::local::stream_protocol> {
public:
	using StreamSocket<asio::local::stream_protocol>::StreamSocket;
};

}
