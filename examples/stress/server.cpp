
#include "TcpServer.h"
#include "TcpSocket.h"
#include "ThreadPool.h"
#include <iostream>

using namespace std;
using namespace boost::asio::ip;

void Main() {
	try {
		auto endpoint = tcp::endpoint(address_v4::from_string("127.0.0.1"), 44440);
		TcpServer server(endpoint);
		server.run([](TcpSocket socket) {
			Buffer buffer(Buffer::MaximumSize);
			while (true) {
				socket.read(&buffer);
				buffer.popFront(socket.write(buffer));
			}
		});
	}
	catch (const std::exception& error) {
		cout << error.what() << endl;
	}
}

int main() {
	Coro coro(Main);
	ThreadPool threadPool(thread::hardware_concurrency());
	threadPool.schedule([&]() {
		coro.resume();
	});
	return 0;
}
