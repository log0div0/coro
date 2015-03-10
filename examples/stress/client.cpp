
#include "TcpSocket.h"
#include "ThreadPool.h"
#include "CoroPool.h"
#include <iostream>

using namespace std;
using namespace std::chrono;
using namespace boost::asio::ip;

auto endpoint = tcp::endpoint(address_v4::from_string("127.0.0.1"), 44440);
uint64_t totalDataSize = 20LL * 1024 * 1024 * 1024;
uint64_t clientCount = 1024;
uint64_t dataSizePerClient = totalDataSize / clientCount;
uint64_t chunkSize = 64 * 1024;
uint64_t chunkCountPerClient = dataSizePerClient / chunkSize;
Buffer outputBuffer(chunkSize);

void ClientRoutine() {
	TcpSocket socket;
	socket.connect(endpoint);

	CoroPool pool;
	pool.exec([&]() {
		for (uint64_t i = 0; i < chunkCountPerClient; i++) {
			socket.write(outputBuffer);
		}
	});
	pool.exec([&]() {
		uint64_t counter = 0;

		Buffer inputBuffer(chunkSize);
		while (true) {
			socket.read(&inputBuffer);
			if ((counter += inputBuffer.usefulDataSize()) == dataSizePerClient) {
				return;
			}
			inputBuffer.clear();
		}
	});
}

void Main() {
	outputBuffer.pushBack(chunkSize);

	auto start = steady_clock::now();
	for (uint64_t i = 0; i < clientCount; ++i) {
		Exec(ClientRoutine);
	}
	Join();
	auto end = steady_clock::now();

	auto elapsed = duration_cast<seconds>(end - start).count();
	cout << fixed;
	cout
		<< "bandwidth = "
		<<  totalDataSize / 1024 / 1024 * 8 / elapsed
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
