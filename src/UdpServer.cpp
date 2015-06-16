
#include "coro/UdpServer.h"
#include "coro/CoroPool.h"

using namespace boost::asio::ip;

UdpServerConnection::UdpServerConnection(UdpServer* server, const boost::asio::ip::udp::endpoint& endpoint)
	: _server(server),
	  _endpoint(endpoint)
{
	auto pair = _server->_inputQueues.emplace(_endpoint, Queue<BufferUniquePtr>());
	assert(pair.second);
	_inputQueue = &pair.first->second;
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

void UdpServerConnection::send(BufferUniquePtr buffer) {
	_server->_socket.send(*buffer, _endpoint);
}

BufferUniquePtr UdpServerConnection::receive() {
	return _inputQueue->pop();
}

UdpServer::UdpServer(const udp::endpoint& endpoint)
	: _socket(endpoint)
{
}

void UdpServer::run(std::function<void(UdpServerConnection)> callback) {
	CoroPool coroPool(true);
	while (true) {
		auto buffer = MallocBuffer();
		udp::endpoint endpoint;
		buffer->pushBack(_socket.receive(buffer.get(), &endpoint));

		auto it = _inputQueues.find(endpoint);
		if (it == _inputQueues.end()) {
			coroPool.exec([&] {
				callback(UdpServerConnection(this, endpoint));
			});
			it = _inputQueues.find(endpoint);
			assert(it != _inputQueues.end());
		}
		it->second.push(std::move(buffer));
	}
}
