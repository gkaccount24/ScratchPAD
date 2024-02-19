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

	//enum class io_device_type
	//{
	//	BufferIO,
	//	FileIO
	//};

 //   template<typename io_device_t>
 //   class io_buffer
 //   {

 //   public:
	//	 io_buffer() = default;
 //       ~io_buffer() = default;

 //   public:
 //       size_t Write(const char* TransmitBuf, size_t Count);
 //       size_t Read(char* ReceiveBuf, size_t Count);

 //       io_buffer& operator<<(const char* TransmitBuf);
 //       io_buffer& operator>>(const char* ReceiveBuf);

 //       io_device_t Data;
 //   };

	///***
	//**** FILEBUF DEVICE
	//****/

	//class io_filebuffer
	//{
	//public:
	//	 io_filebuffer(io_filebuffer&& FileBuffer);
	//	 io_filebuffer();
	//	 io_filebuffer(const char* FileBufDiskPath);
	//	~io_filebuffer();

 //   public: // REIMPLEMENTED PURE VIRTUAL METHODS
	//	virtual size_t Write(const char* Bytes, size_t Count);
	//	virtual size_t Read(char* ReceiveBuf, size_t Count);

 //   public:
	//	bool Open(const char* Path, DWORD DesiredAccess,
 //                 DWORD ShareAccess, DWORD CreateFlags,
 //                 DWORD AttributeFlags, LPSECURITY_ATTRIBUTES Security = 0);
 //       bool OpenForReading(const char* Path);
 //       bool OpenForWriting(const char* Path, bool OverWrite);
 //       bool OpenForReadAndWrite(const char* Path, bool OverWrite);
 //       bool Create(const char* Path, bool OverWrite);
 //       void Close();

 //   public:
 //       const char* BeginBuffer();
 //       const char* EndBuffer();

 //   private:
 //       bool TryToSetASCII();
 //       bool TryToSetUTF8BOM();
 //       bool TryToSetUTF16BOM();
 //       bool TryToSetBOMAndFileEncoding();

 //   public:
 //       void GetDirectoryContents(const char* Path, vector<file*>& Files);

 //   public:
 //       void StatFile();

 //       size_t GetSizeOnDisk();

 //   private:
 //       bool SyncInternalBufferWithDisk();
 //       bool ParseFilename();

	//private:
 //       HANDLE HandleId;

 //       DWORD DesiredAccess;
 //       DWORD ShareAccess;
 //       DWORD CreateFlags;
 //       DWORD AttributeFlags;

 //       file_encodings Encoding;
 //       file_endianness Endianness;
 //       bool EncodingAndBOMSet;

 //       string AbsPath;
 //       string Name;

 //   public:

 //       bool IsDir;
 //       bool IsSync;
 //       bool IsOpen;
 //       bool IsLoaded;

 //       WIN32_FILE_ATTRIBUTE_DATA Stat;
 //   };
}

#endif