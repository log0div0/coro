
#include "coro/UnixSocket.h"
#include "coro/IoService.h"

using namespace boost::asio::local;

UnixSocket::UnixSocket(): UnixSocket(stream_protocol::socket(*IoService::current())) {

}

UnixSocket::UnixSocket(stream_protocol::socket socket): Stream(std::move(socket)) {

}


UnixSocket::UnixSocket(UnixSocket&& other): Stream(std::move(other)) {

}

UnixSocket& UnixSocket::operator=(UnixSocket&& other) {
	Stream::operator=(std::move(other));
	return *this;
}

void UnixSocket::connect(const std::string& endpoint) {
	AsioTask1 task;
	_handle.async_connect(endpoint, task.callback());
	task.wait(_handle);
}
