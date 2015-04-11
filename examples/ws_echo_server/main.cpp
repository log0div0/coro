
#include "ThreadPool.h"
#include "CoroPool.h"
#include "TcpServer.h"
#include "TcpSocket.h"
#include "WsProtocol.h"
#include "NonBlockingBufferPool.h"
#include <iostream>

using namespace std;
using namespace boost::asio::ip;

class WsEchoSession {
public:
	WsEchoSession(TcpSocket socket): _socket(move(socket))
	{
		cout << "WsEchoSession" << endl;
	}
	~WsEchoSession() {
		cout << "~WsEchoSession" << endl;
	}

	void doHandshake() {
		auto outputBuffer = MallocUnique();
		_inputBuffer->popFront(
			_wsProtocol.doHandshake(
				_socket.iterator(*_inputBuffer),
				_socket.iterator(),
				*outputBuffer
			)
		);
		_socket.write(*outputBuffer);
	}

	void printMessage(const WsMessage& message) {
		if (message.opCode() == WsMessage::OpCode::Text) {
			copy(message.payloadBegin(), message.payloadEnd(),
				ostream_iterator<char>(cout));
			cout << endl;
		}
	}

	void run() {
		try {
			doHandshake();

			while (true) {
				WsMessage message = _wsProtocol.readMessage(
					_socket.iterator(*_inputBuffer),
					_socket.iterator()
				);

				if (message.opCode() == WsMessage::OpCode::Close) {
					auto outputBuffer = MallocUnique();
					_wsProtocol.writeMessage(WsMessage::OpCode::Close, *outputBuffer);
					_socket.write(*outputBuffer);
					return;
				}

				printMessage(message);

				auto outputBuffer = MallocUnique();
				// копируем payload
				outputBuffer->assign(message.payloadBegin(), message.payloadEnd());
				// запаковываем в websockets
				_wsProtocol.writeMessage(message.opCode(), *outputBuffer);
				// отправляем
				_socket.write(*outputBuffer);

				_inputBuffer->popFront(message.end());
			}
		}
		catch (const exception& error) {
			cout << error.what() << endl;
		}
	}

private:
	TcpSocket _socket;
	WsServerProtocol _wsProtocol;
	BufferUniquePtr _inputBuffer = MallocUnique();
};

void StartAccept() {
	auto endpoint = tcp::endpoint(address_v4::from_string("127.0.0.1"), 44442);
	TcpServer server(endpoint);
	server.run([](TcpSocket socket) {
		WsEchoSession session(std::move(socket));
		session.run();
	});
}

int main() {
	ThreadPool threadPool(thread::hardware_concurrency());

	Coro coro(StartAccept, &threadPool);
	coro.strand()->post([&] {
		coro.resume();
	});

	threadPool.sync();
	return 0;
}
