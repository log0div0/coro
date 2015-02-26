
#pragma once

#include "Buffer.h"

template <typename Iterator>
Iterator ReadWsPayloadLength(const Iterator& begin, const Iterator& end, uint64_t* integer) {
	Iterator it = begin;

	uint64_t payloadLength = *it & 0x7f;
	if (payloadLength <= 0x7d) {
		*integer = payloadLength;
		it += 1;
	} else if (payloadLength == 0x7e) {
		++it;
		*integer = 0;
		std::copy(it, it + 2,
			std::reverse_iterator<uint8_t*>(reinterpret_cast<uint8_t*>(integer) + 2));
		it += 2;
	} else {
		++it;
		std::copy(it, it + 4,
			std::reverse_iterator<uint8_t*>(reinterpret_cast<uint8_t*>(integer) + 4));
		it += 4;
	}

	return it;
}

void WriteWsPayloadLength(Buffer& buffer, uint64_t integer);
