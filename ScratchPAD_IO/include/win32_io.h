#ifndef WIN32_IO_H
#define WIN32_IO_H

#include "defs.h"

#include <string>

using std::begin;
using std::endl;

enum file_properties
{
    RowNumber = 0,
    Name = 1,
    LastWriteTime = 2,
    Type = 3,
    Size = 4,
    CreationTime = 5,
    LastAccessTime = 6,
    Count = 7
};

enum class file_encodings 
{
    ASCIIEncoding = 0,
    UTF8Encoding = 1,
    UTF16Encoding = 2,
    UnknownEncoding = 3
};

enum class file_bom_endianness
{
    BigEndian = 0,
    LittleEndian = 1,
    Unknown = 2
};

enum class file_endianness
{
    BigEndian = 0,
    LittleEndian = 1,
    Unknown = 2
};

#define FileEncoding(Encoding)(file_encodings::Encoding)
#define FileBOMEndianness(BOMEndianness)(file_bom_endianness::BOMEndianness)
#define FileEndianness(Endianness)(file_endianness::Endianness)

#define HighByte32(Bitmask)(((uint32_t) Bitmask) & 0xFF000000)
#define LowByte32(Bitmask)(((uint32_t) Bitmask) & 0x000000FF)

#define HighNibble32(Bitmask)(((uint32_t) Bitmask) & 0xF0000000)
#define LowNibble32(Bitmask)(((uint32_t) Bitmask) & 0x0000000F)

#define HighNibble8(Bitmask)(((uint8_t) Bitmask) & 0xF0)
#define LowNibble8(Bitmask)(((uint8_t) Bitmask) & 0x0F)

class file 
{

public:
     file() = default;
    ~file() = default;

    bool Open(const char* Path, DWORD DesiredAccess,
              DWORD ShareAccess, DWORD CreateFlags,
              DWORD AttributeFlags, LPSECURITY_ATTRIBUTES Security = 0);

    bool OpenForReading(const char* Path);
    bool OpenForWriting(const char* Path, bool OverWrite);
    bool OpenForReadAndWrite(const char* Path, bool OverWrite);

    bool Create(const char* Path, bool OverWrite);

    void Close();

public:
    const char* BeginBuffer();
    const char* EndBuffer();

private:
    bool TryToSetASCII();
	bool TryToSetUTF8BOM();
    bool TryToSetUTF16BOM();
    bool TryToSetBOMAndFileEncoding();

public:
    void GetDirectoryContents(const char* Path, vector<file*>& Files);

public:
    void StatFile();

    size_t GetSizeOnDisk();

private:
    bool SyncInternalBufferWithDisk();
    bool ParseFilename();

private:
    file(const file& Rfile);
    file(file&& Rfile);

private:
    HANDLE Id;

    DWORD DesiredAccess;
    DWORD ShareAccess;
    DWORD CreateFlags;
    DWORD AttributeFlags;

    file_encodings Encoding;
    file_bom_endianness BOMEndianness;
    file_endianness Endianness;

    bool EncodingAndBOMSet;

    string AbsPath;
    string Name;

    string Buffer;
    size_t SizeOnDisk;

public:

    bool IsDir;
    bool IsSync;
    bool IsOpen;
    bool IsLoaded;

    WIN32_FILE_ATTRIBUTE_DATA Stat;
};

void ConsoleNotice(const char* Message);
void ConsoleWarning(const char* Message);
void ConsoleError(const char* Message);

#endif