
#include "ThreadPool.h"
#include "Coro.h"
#include "CoroUtils.h"
#include "TcpServer.h"
#include "WsProtocol.h"
#include "NetworkIterator.h"
#include "NonBlockingBufferPool.h"
#include <iostream>

using namespace std;

class WsEchoSession {
public:
	WsEchoSession(TcpSocket socket)
		: _socket(std::move(socket)),
		  _inputBuffer(1000)
	{
		cout << "WsEchoSession" << endl;
	}
	~WsEchoSession() {
		cout << "~WsEchoSession" << endl;
	}

	void doHandshake() {
		auto outputBuffer = MakeBufferUnique<1000>();
		_inputBuffer.popFront(
			_wsProtocol.doHandshake(
				NetworkIterator(_socket, _inputBuffer),
				NetworkIterator(),
				*outputBuffer
			)
		);
		_socket.sendData(*outputBuffer);
	}

	void printMessage(const WsMessage& message) {
		if (message.opCode() == WsMessage::OpCode::Text) {
			copy(message.payloadBegin(), message.payloadEnd(),
				ostream_iterator<char>(cout));
			cout << endl;
		}
	}

	void operator()() {
		try {
			doHandshake();

			while (true) {
				WsMessage message;
				_wsProtocol.readMessage(
					NetworkIterator(_socket, _inputBuffer),
					NetworkIterator(),
					&message
				);

				printMessage(message);

				auto outputBuffer = MakeBufferUnique<1000>();
				// копируем payload
				outputBuffer->assign(message.payloadBegin(), message.payloadEnd());
				// запаковываем в websockets
				_wsProtocol.writeMessage(message.opCode(), *outputBuffer);
				// отправляем
				_socket.sendData(*outputBuffer);

				_inputBuffer.popFront(message.end());
			}
		}
		catch (const exception& e) {
			cout << e.what() << endl;
		}
	}

private:
	TcpSocket _socket;
	WsServerProtocol _wsProtocol;
	Buffer _inputBuffer;
};

void StartAccept() {
	auto endpoint = TcpEndpoint(IPv4Address::from_string("127.0.0.1"), 44442);
	TcpServer server(endpoint);
	server.run<WsEchoSession>();
}

int main() {
	Coro coro(StartAccept);
	ThreadPool threadPool(std::thread::hardware_concurrency());
	threadPool.schedule([&]() {
		coro.resume();
	});
	threadPool.sync();
	return 0;
}
