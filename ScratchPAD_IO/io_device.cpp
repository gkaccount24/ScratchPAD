#include "./include/stream_io.h"

namespace scratchpad
{				 
	/***
	**** PURE VIRTUAL IO_DEVICE
	****/

	io_device::io_device(io_device_type DeviceType):
		Type(DeviceType)
	{

	}

	io_device::~io_device()
	{

	}

	/***
	**** IO_STRINGBUFFER
	****/

	io_stringbuffer::io_stringbuffer(): 
		io_device(io_device_type::BufferIO)
	{

	}

	io_stringbuffer::io_stringbuffer(string StringBuffer):
		io_device(io_device_type::BufferIO),
		Buffer(move(StringBuffer))
	{

	}

	io_stringbuffer::~io_stringbuffer()
	{
		Buffer.clear();
		Buffer.shrink_to_fit();
	}

	void io_stringbuffer::Write(const char* Bytes, size_t Count)
	{

	}

	size_t io_stringbuffer::Read(char* ReceiveBuf, size_t Count)
	{

	}

	/***
	**** FILEBUF DEVICE
	****/

	io_filebuffer::io_filebuffer(scratchpad::file& IOFile): 
		io_device(io_device_type::FileIO),

		// parameters
		File(IOFile)
	{

	}

	io_filebuffer::~io_filebuffer()
	{

	}

	void Write(const char* Bytes, size_t Count);
	size_t Read(char* ReceiveBuf, size_t Count);


}