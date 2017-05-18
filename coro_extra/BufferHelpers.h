
#pragma once


#include "coro_extra/Buffer.h"


template <typename Iterator, typename POD>
Iterator ReadPOD(Iterator it, const Iterator& end, POD* pod) {
	std::copy(it, it + sizeof(POD), reinterpret_cast<uint8_t*>(pod));
	return it + sizeof(POD);
}


template <typename POD>
void WritePOD(Buffer& buffer, const POD& pod) {
	buffer.pushFront(
		(uint8_t*)&pod,
		(uint8_t*)&pod + sizeof(POD)
	);
}


template <typename Iterator>
Iterator ReadMultibyteInteger(Iterator it, const Iterator& end, uint64_t* integer) {
	*integer = 0;
	while (true) {
		*integer = ((*integer) << 7) | (*it & ~0x80);
		if (!(*it++ & 0x80)) {
			break;
		}
	}

	return it;
}


inline void WriteMultibyteInteger(Buffer& buffer, uint64_t integer) {
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

