#ifndef SCRATCHPAD_LOGGER_H
#define SCRATCHPAD_LOGGER_H

#include "io_device.h"
#include "stream_io.h"

/***
*** C++ INCLUDES
***/

#include <string>

namespace scratchpad
{
	class logger
	{

	public:
		logger(io_device& LoggerIODevice);
		~logger();

	public:
		void ConsoleInfo(const char* Message);
		void ConsoleNotice(const char* Message);
		void ConsoleWarning(const char* Message);
		void ConsoleError(const char* Message);

	private:
		// stream_io* IOStream;

	};
}

#endif
