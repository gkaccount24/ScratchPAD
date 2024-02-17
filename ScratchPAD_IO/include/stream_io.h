#ifndef SCRATCHPAD_STREAM_IO_H
#define SCRATCHPAD_STREAM_IO_H

#include "io_device.h"

/***
*** C++ INCLUDES
***/

#include <string>

using std::string;

namespace scratchpad
{
	enum class stream_io_type
	{
		BufferIO,
		ConsoleIO,
		FileIO
	};

	class stream_io
	{

	public:
		stream_io(io_stringbuffer&& StreamBuffer);
		~stream_io();

	public:
		size_t Write(const char* Bytes, size_t Count);
		size_t Read(char* ReceiveBuf, size_t Count);

	private:
		io_stringbuffer Buffer;

	public:

		string::iterator BeginStream;
		string::iterator EndStream;
		string::iterator Cursor;

		size_t WSSkipped;
		size_t BytesRead;

		size_t Row;
		size_t Col;

		size_t LineCount;
		size_t ByteOffset;

		size_t BufferSize;
		size_t BytesAvailable;
	};
}

#endif