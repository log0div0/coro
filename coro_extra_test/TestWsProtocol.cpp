#include "coro/IoService.h"
#include "coro/CoroPool.h"
#include "coro_extra/TcpServer.h"
#include "coro_extra/TcpSocket.h"
#include "coro_extra/WsProtocol.h"
#include <iostream>
#include <catch.hpp>

using namespace std;
using namespace asio::ip;
using namespace coro;

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
		Buffer outputBuffer;
		_inputBuffer.popFront(
			_wsProtocol.doHandshake(
				_socket.iterator(_inputBuffer),
				_socket.iterator(),
				outputBuffer
			)
		);
		_socket.write(outputBuffer);
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
					_socket.iterator(_inputBuffer),
					_socket.iterator()
				);

				if (message.opCode() == WsMessage::OpCode::Close) {
					Buffer outputBuffer;
					_wsProtocol.writeMessage(WsMessage::OpCode::Close, outputBuffer);
					_socket.write(outputBuffer);
					return;
				}

				printMessage(message);

				Buffer outputBuffer;
				// копируем payload
				outputBuffer.assign(message.payloadBegin(), message.payloadEnd());
				// запаковываем в websockets
				_wsProtocol.writeMessage(message.opCode(), outputBuffer);
				// отправляем
				_socket.write(outputBuffer);

				_inputBuffer.popFront(message.end());
			}
		}
		catch (const exception& error) {
			cout << error.what() << endl;
		}
	}

private:
	TcpSocket _socket;
	WsProtocol _wsProtocol;
	Buffer _inputBuffer;
};

TEST_CASE("wsprotocol test") {
	auto endpoint = tcp::endpoint(address_v4::from_string("127.0.0.1"), 44442);
	TcpServer server(endpoint);
	server.run([](TcpSocket socket) {
		WsEchoSession session(std::move(socket));
		session.run();
	});
}

