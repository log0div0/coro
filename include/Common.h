
#pragma once

#include <vector>

#ifdef _MSC_VER
#define THREAD_LOCAL __declspec(thread)
#else
#define THREAD_LOCAL __thread
#endif

template <typename T>
std::vector<uint8_t> ToBinaryVector(const T& t) {
	return { (uint8_t*)&t, (uint8_t*)&t + sizeof(T) };
}

