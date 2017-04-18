
#include "coro_extra/UdpServer.h"
#include "coro/CoroPool.h"

using namespace asio::ip;

namespace coro {

UdpServerConnection::UdpServerConnection(UdpServer* server, std::unique_ptr<Buffer> firstPacket,
	const asio::ip::udp::endpoint& endpoint)
	: _server(server),
	  _endpoint(endpoint)
{
	auto pair = _server->_inputQueues.emplace(_endpoint, Queue<std::unique_ptr<Buffer>>());
	assert(pair.second);
	_inputQueue = &pair.first->second;
	_inputQueue->push(std::move(firstPacket));
}

UdpServerConnection::UdpServerConnection(UdpServerConnection&& other)
	: _server(other._server),
	  _endpoint(other._endpoint),
	  _inputQueue(other._inputQueue)
{
	other._server = nullptr;
	other._inputQueue = nullptr;
}

UdpServerConnection::~UdpServerConnection() {
	if (_server) {
		assert(_server->_inputQueues.erase(_endpoint));
	}
}

void UdpServerConnection::send(std::unique_ptr<Buffer> buffer) {
	_server->_socket.send(*buffer, _endpoint);
}

std::unique_ptr<Buffer> UdpServerConnection::receive() {
	return _inputQueue->pop();
}

UdpServer::UdpServer(const udp::endpoint& endpoint)
	: _socket(endpoint)
{
}

UdpServerConnection UdpServer::accept() {
	while (true) {
		std::unique_ptr<Buffer> buffer(new Buffer);
		udp::endpoint endpoint;
		buffer->pushBack(_socket.receive(buffer.get(), &endpoint));

		auto it = _inputQueues.find(endpoint);
		if (it == _inputQueues.end()) {
			return UdpServerConnection(this, std::move(buffer), endpoint);
		}
		it->second.push(std::move(buffer));
	}
}

void UdpServer::run(std::function<void(UdpServerConnection)> callback) {
	CoroPool coroPool;
	while (true) {
		auto socket = accept();
		coroPool.exec([&] {
			callback(std::move(socket));
		});
	}
}

}