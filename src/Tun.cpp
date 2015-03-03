
#include "Tun.h"
#include "ThreadPool.h"
#include "Format.h"
#include <linux/if_tun.h>


Tun::Tun(const std::string& name)
	: IoHandle(boost::asio::posix::stream_descriptor(ThreadPool::current()->ioService())),
	  _name(name)
{
	int file = ::open("/dev/net/tun", O_RDWR);
	if (file < 0) {
		throw std::runtime_error("Failed to open file /dev/net/tun");
	}

	ifreq ifr;
	memset(&ifr, 0, sizeof(ifr));
	snprintf(ifr.ifr_name, IFNAMSIZ, "%s", _name.c_str());
	ifr.ifr_flags = IFF_TUN | IFF_NO_PI;

	if (ioctl(file, TUNSETIFF, &ifr) < 0) {
		throw std::runtime_error(Format("Failed to create virtual network interface %1%", _name));
	}

	_handle.assign(file);
}
