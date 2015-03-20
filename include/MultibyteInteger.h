
#pragma once


#include "Buffer.h"


template <typename Iterator>
Iterator ReadMultibyteInteger(const Iterator& begin, const Iterator& end, uint64_t* integer) {
	Iterator it = begin;

	*integer = 0;
	while (true) {
		*integer = ((*integer) << 7) | (*it & ~0x80);
		if (!(*it++ & 0x80)) {
			break;
		}
		if (it == end) {
			throw std::runtime_error("ReadMultibyteInteger");
		}
	}

	return it;
}


void WriteMultibyteInteger(Buffer& buffer, uint64_t integer);
