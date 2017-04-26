
#include "coro_extra/TcpSocket.h"

using namespace asio::ip;

namespace coro {

size_t TcpSocket::write(const Buffer& buffer) {
	AsioTask2<size_t> task;
	asio::async_write(_handle, buffer.usefulData(), task.callback());
	return task.wait(_handle);
}

size_t TcpSocket::readSome(Buffer* buffer) {
	AsioTask2<size_t> task;
	_handle.async_read_some(buffer->freeSpace(), task.callback());
	return task.wait(_handle);
}

void TcpSocket::close() {
	_handle.shutdown(tcp::socket::shutdown_both);
	_handle.close();
}

}
