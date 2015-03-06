
#include "TcpServer.h"
#include "TcpSocket.h"
#include "ThreadPool.h"

using namespace std;
using namespace boost::asio::ip;

void Main() {
	auto endpoint = tcp::endpoint(address_v4::from_string("127.0.0.1"), 44442);
	TcpServer server(endpoint);
	server.run([](TcpSocket socket) {
		Buffer buffer;
		while (true) {
			socket.read(&buffer);
			buffer.popFront(socket.write(buffer));
		}
	});
}

int main() {
	Coro coro(Main);
	ThreadPool threadPool(thread::hardware_concurrency());
	threadPool.schedule([&]() {
		coro.resume();
	});
	return 0;
}
