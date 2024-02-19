#ifndef SCRATCHPAD_STREAM_IO_H
#define SCRATCHPAD_STREAM_IO_H

#include "io_device.h"

/***
*** C++ INCLUDES
***/

#include <fstream>
#include <sstream>
#include <string>

using std::streambuf;
using std::iostream;
using std::stringstream;
using std::fstream;
using std::string;

//namespace scratchpad
//{
//	class stream_io
//	{
//
//	public:
//		stream_io(fstream&& SourceFile);
//		~stream_io();
//
//	public:
//
//		size_t Write(const char* Bytes, size_t Count);
//		size_t Read(char* ReceiveBuf, size_t Count);
//
//		size_t ReadUntil(char* ReceiveBuf, size_t ReceiveBufSize, char Delim);
//		size_t ReadUntilNL(char* ReceiveBuf, size_t ReceiveBufSize);
//
//	private:
//		bool IsNL();
//		bool IsWS();
//		void TrimWS();
//
//	private:
//		iostream Stream;
//
//	public:
//		size_t WSSkipped;
//		size_t BytesRead;
//		size_t BytesWritten;
//		size_t LineCount;
//
//		size_t Row;
//		size_t Col;
//	};
//}

#endif