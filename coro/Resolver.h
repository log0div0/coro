
#pragma once

#include "coro/IoService.h"
#include "coro/AsioTask.h"

template <typename InternetProtocol>
class Resolver {
public:
	typedef boost::asio::ip::basic_resolver<InternetProtocol> Impl;
	typedef typename Impl::iterator Iterator;
	typedef typename Impl::query Query;

	Resolver(): _handle(*IoService::current()) {}

	Iterator resolve(const Query& query) {
		AsioTask2<Iterator> task;
		_handle.async_resolve(query, task.callback());
		return task.wait(_handle);
	}

private:
	Impl _handle;
};

typedef Resolver<boost::asio::ip::udp> UdpResolver;
typedef Resolver<boost::asio::ip::tcp> TcpResolver;
