#ifndef SCRATCHPAD_IO_DEVICE_H
#define SCRATCHPAD_IO_DEVICE_H

/***
*** SCRATCHPAD INCLUDES
***/

#include "file.h"

/***
*** C++ INCLUDES
***/

#include <string>

using std::string;

namespace scratchpad
{
	/***
	**** PURE VIRTUAL IO_DEVICE
	****/

	enum class io_device_type
	{
		BufferIO,
		ConsoleIO,
		FileIO
	};

	class io_device
	{
	protected:
				 io_device(io_device_type DeviceType);
		virtual ~io_device();

	public:
		virtual void Write(const char* Bytes, size_t Count) = 0;
		virtual size_t Read(char* ReceiveBuf, size_t Count) = 0;

	protected:
		io_device_type Type;

	};

	/***
	**** IO_STRINGBUFFER
	****/

	class io_stringbuffer: public io_device
	{
	public:
				 io_stringbuffer();
				 io_stringbuffer(string StringBuffer);
		virtual ~io_stringbuffer();

		virtual void Write(const char* Bytes, size_t Count);
		virtual size_t Read(char* ReceiveBuf, size_t Count);

	private:
		string Buffer;

	};

	/***
	**** FILEBUF DEVICE
	****/

	class io_filebuffer: public io_device
	{
	public:
				 io_filebuffer(scratchpad::file& IOFile);
		virtual ~io_filebuffer();

		virtual void Write(const char* Bytes, size_t Count);
		virtual size_t Read(char* ReceiveBuf, size_t Count);

	private:
		scratchpad::file& File;

	};
}

#endif