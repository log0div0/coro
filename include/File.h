
#pragma once

#include "IoHandle.h"

class File: public IoHandle<boost::asio::posix::stream_descriptor> {
public:
	File(const std::string& name);
};