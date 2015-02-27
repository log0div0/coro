
#include "WsPayloadLength.h"


void WriteWsPayloadLength(Buffer& buffer, uint64_t integer) {
	if (integer <= 0x7d) {
		buffer.pushFront(1);
		buffer.front() = integer & 0x7f;
	} else if (integer <= 0xffff) {
		auto it = std::reverse_iterator<uint8_t*>(reinterpret_cast<uint8_t*>(&integer) + 2);
		buffer.pushFront(it, it + 2);
		buffer.pushFront(1);
		buffer.front() = 0x7e;
	} else {
		auto it = std::reverse_iterator<uint8_t*>(reinterpret_cast<uint8_t*>(&integer) + 4);
		buffer.pushFront(it, it + 4);
		buffer.pushFront(1);
		buffer.front() = 0x7f;
	}
}