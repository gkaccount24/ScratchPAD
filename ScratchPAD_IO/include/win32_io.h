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

struct file 
{
    HANDLE _Id;

    DWORD _DesiredAccess;
    DWORD _ShareAccess;
    DWORD _CreateFlags;
    DWORD _AttributeFlags;

    file_encodings Encoding;
    file_bom_endianness BOMEndianness;
    file_endianness Endianness;

    bool EncodingAndBOMSet;

    string _AbsPath;
    string _Name;

    string _Buffer;
    size_t _SizeOnDisk;

    bool   _IsDir;
    bool   _IsSync;
    bool   _IsOpen;
    bool   _IsLoaded;

    WIN32_FILE_ATTRIBUTE_DATA Stat;
};

void ConsoleNotice(const char* Message);
void ConsoleWarning(const char* Message);
void ConsoleError(const char* Message);

size_t GetSizeOnDisk(const char* Path);
size_t GetSizeOnDisk(file* Node);
void StatFile(file* Node);

void CloseFile(file* File);

file* Open(const char* Path, 
              DWORD DesiredAccess, 
              DWORD ShareAccess, 
              DWORD CreateFlags, 
              DWORD AttributeFlags, 
              LPSECURITY_ATTRIBUTES SecAttributes = 0);

file* OpenForReading(const char* Path);
file* OpenForWriting(const char* Path, bool OverWrite = false);
file* OpenForReadAndWrite(const char* Path, bool OverWrite = false);
file* Create(const char* Path, bool OverWrite = false);

bool TryToSetASCII(file* File);
bool TryToSetUTF8BOM(file* File);
bool TryToSetUTF16BOM(file* File);
bool TryToSetBOMAndFileEncoding(file* File);

void GetDirectoryContents(const char* Path, vector<file*>& Files);

#endif