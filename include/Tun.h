
#pragma once

#include "IoHandle.h"

class Tun: public IoHandle<boost::asio::posix::stream_descriptor> {
public:
	Tun(const std::string& name);

private:
	std::string _name;
};
