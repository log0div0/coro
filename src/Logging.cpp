
#include "Logging.h"

#include <boost/log/core.hpp>
#include <boost/log/expressions.hpp>
#include <boost/log/utility/setup/console.hpp>

namespace logging = boost::log;
namespace keywords = boost::log::keywords;
namespace expr = boost::log::expressions;

void InitDefaultLogging() {
	logging::add_console_log(std::clog, keywords::format = (
		expr::stream
			<< "[" << logging::trivial::severity << "] "
			<< expr::smessage
	));
	logging::core::get()->set_filter(logging::trivial::severity >= logging::trivial::trace);
}

