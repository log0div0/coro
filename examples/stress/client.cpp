
#include "TcpSocket.h"
#include "ThreadPool.h"
#include "CoroPool.h"
#include <iostream>

using namespace std;
using namespace boost::asio::ip;

auto endpoint = tcp::endpoint(address_v4::from_string("127.0.0.1"), 44442);
uint64_t totalDataSize = 1024 * 1024 * 1024;
uint64_t clientCount = 1024;
uint64_t dataSizePerClient = totalDataSize / clientCount;
uint64_t chunkSize = 1024;
uint64_t chunkCountPerClient = dataSizePerClient / chunkSize;

void ClientRoutine() {
	TcpSocket socket;
	socket.connect(endpoint);

	CoroPool pool;
	pool.exec([&]() {
		Buffer buffer;
		buffer.pushBack(chunkSize);
		for (auto i = 0; i < chunkCountPerClient; i++) {
			socket.write(buffer);
		}
	});
	pool.exec([&]() {
		uint64_t counter = 0;

		Buffer buffer(chunkSize);
		while (true) {
			socket.read(&buffer);
			if ((counter += buffer.usefulDataSize()) == dataSizePerClient) {
				return;
			}
			buffer.clear();
		}
	});
}

void Main() {
	auto start = std::chrono::system_clock::now();
	for (auto i = 0; i < clientCount; ++i) {
		Exec(ClientRoutine);
	}
	Join();
	auto end = std::chrono::system_clock::now();

	std::chrono::duration<double> duration = end - start;
	cout << fixed;
	cout
		<< "bandwidth = "
		<<  totalDataSize / 1024 / 1024 * 8 / duration.count()
		<< "Mb/s"
		<< endl;
}

int main() {
	Coro coro(Main);
	ThreadPool threadPool(thread::hardware_concurrency());
	threadPool.schedule([&]() {
		coro.resume();
	});
	return 0;
}
