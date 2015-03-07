
#include "File.h"
#include "ThreadPool.h"

using boost::system::system_error;

File::File(const std::string& name)
	: IoHandle({
		ThreadPool::current()->ioService(),
		open(name.c_str(), O_RDWR)
	})
{
}
