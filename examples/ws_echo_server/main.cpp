
#include "ThreadPool.h"
#include "Coro.h"
#include "CoroUtils.h"
#include "TcpServer.h"
#include "WsProtocol.h"
#include "NetworkIterator.h"
#include <iostream>

using namespace std;

void SessionRoutine(TcpSocket socket) {
	cout << "start SessionRoutine" << endl;

	try {
		WsServerProtocol protocol;
		Buffer input(1000), output(1000);

		input.popFront(
			protocol.doHandshake(NetworkIterator(socket, input), NetworkIterator(), output)
		);
		output.popFront(
			socket.sendData(output)
		);

		while (true) {
			WsMessage message;
			protocol.readMessage(NetworkIterator(socket, input), NetworkIterator(), &message);

			if (message.opCode() == WsMessage::OpCode::Text) {
				copy(message.payloadBegin(), message.payloadEnd(),
					ostream_iterator<char>(cout));
				cout << endl;
			}

			output.assign(message.payloadBegin(), message.payloadEnd());
			protocol.writeMessage(message.opCode(), output);

			input.popFront(message.end());
			output.popFront(socket.sendData(output));

		}
	}
	catch (const exception& e) {
		cout << e.what() << endl;
	}

	cout << "stop SessionRoutine" << endl;
}

void ServerRoutine() {
	auto endpoint = TcpEndpoint(IPv4Address::from_string("127.0.0.1"), 44442);
	TcpServer server(endpoint);
	server.run(SessionRoutine);
}

int main() {
	Coro coro(ServerRoutine);
	ThreadPool threadPool;
	threadPool.schedule([&]() {
		coro.resume();
	});
	threadPool.run();
	return 0;
}
