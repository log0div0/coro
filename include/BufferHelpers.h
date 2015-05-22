
#pragma once


#include "Buffer.h"


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


void WriteMultibyteInteger(Buffer& buffer, uint64_t integer);
