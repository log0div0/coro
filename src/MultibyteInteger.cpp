
#include "MultibyteInteger.h"


void WriteMultibyteInteger(Buffer& buffer, uint64_t integer) {
	for (uint8_t i = 0; ; ++i) {
		buffer.pushFront(1);
		if (i == 0) {
			buffer.front() = integer & 0x7f;
		} else {
			buffer.front() = 0x80 | (integer & 0x7f);
		}
		integer >>= 7;
		if (integer == 0) {
			break;
		}
	}
}
