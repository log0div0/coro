
#include "TcpSocket.h"
#include "ThreadPool.h"
#include "CoroPool.h"
#include <iostream>

using namespace std;
using namespace boost::asio::ip;

void Main() {
	auto endpoint = tcp::endpoint(address_v4::from_string("127.0.0.1"), 44442);
	uint64_t totalDataSize = 1024 * 1024 * 1024;
	uint64_t coroCount = 128;
	uint64_t dataSizePerCoro = totalDataSize / coroCount;
	uint64_t chunkSize = 1024;

	auto start = std::chrono::system_clock::now();
	for (uint64_t i = 0; i < coroCount; ++i) {
		Exec([&]() {
			TcpSocket socket;
			socket.connect(endpoint);
			Buffer buffer;
			buffer.pushBack(chunkSize);
			for (uint64_t i = 0; i < dataSizePerCoro; i += chunkSize) {
				buffer.popFront(socket.write(buffer));
				while (buffer.usefulDataSize() != chunkSize) {
					socket.read(&buffer);
				}
			}
		});
	}
	Join();
	auto end = std::chrono::system_clock::now();

	std::chrono::duration<double> duration = end - start;
	cout << fixed;
	cout << "bandwidth " <<  totalDataSize / 1024 / 1024 * 8 / duration.count() << "Mb/s" << endl;
}

int main() {
	Coro coro(Main);
	ThreadPool threadPool(thread::hardware_concurrency());
	threadPool.schedule([&]() {
		coro.resume();
	});
	return 0;
}
