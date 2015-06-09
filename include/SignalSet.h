
#pragma once

#include "AsioTask.h"

class SignalSet {
public:
	SignalSet(const std::initializer_list<int32_t>& signals);

	int32_t wait();

private:
	boost::asio::signal_set _handle;
};