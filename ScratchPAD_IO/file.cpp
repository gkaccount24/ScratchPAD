#include "./include/win32_io.h"

namespace scratchpad
{
    file::file():
        HandleId(nullptr),
        DesiredAccess(0),
        ShareAccess(0),
        CreateFlags(0),
        AttributeFlags(0),
        Encoding(FileEncoding(Unknown)),
        Endianness(FileEndianness(Unknown)),
        EncodingAndBOMSet(false),

        // empty strings
        AbsPath(""),
        Name(""),

        // empty buffer
        Buffer(""),
        SizeOnDisk(0),

        IsDir(false),
        IsSync(false),
        IsOpen(false),
        IsLoaded(false),
        Stat(WIN32_FILE_ATTRIBUTE_DATA { })
    {

    }

    file::file(const char* Path):
        HandleId(nullptr),
        DesiredAccess(0),
        ShareAccess(0),
        CreateFlags(0),
        AttributeFlags(0),
        Encoding(FileEncoding(Unknown)),
        Endianness(FileEndianness(Unknown)),
        EncodingAndBOMSet(false),

        // empty strings
        AbsPath(""),
        Name(""),

        // empty buffer
        Buffer(""),
        SizeOnDisk(0),

        IsDir(false),
        IsSync(false),
        IsOpen(false),
        IsLoaded(false),
        Stat(WIN32_FILE_ATTRIBUTE_DATA { })

    {
        if(!OpenForReading(Path))
        {
            // log & report error
            // set bad state;
        }
    }

    file::~file()
    {
        if(HandleId)
        {
            CloseHandle(HandleId);
            HandleId = nullptr;
        }

        Buffer.clear();
        Buffer.shrink_to_fit();

        SizeOnDisk = 0;

        IsDir = false;
        IsSync = false;
        IsLoaded = false;
        IsOpen = false;

        Stat = { };
    }

    void file::StatFile()
    {
        GET_FILEEX_INFO_LEVELS RequestedFileInfo = GetFileExInfoStandard;

        size_t Result = 0;
        size_t Size = 0;

        Result = GetFileAttributesExA(AbsPath.c_str(), RequestedFileInfo, &Stat);

        if(Result == 0)
        {
            DWORD ErrorCode = GetLastError();

            // LogError("failed to get file attributes.\n", ErrorCode);
        }
        else
        {
            LARGE_INTEGER FileSize = { };

            FileSize.LowPart = Stat.nFileSizeLow;
            FileSize.HighPart = Stat.nFileSizeHigh;

            Size = FileSize.QuadPart;
            SizeOnDisk = Size;
        }
    }

    //size_t file::GetSizeDisk(const char* Path)
    //{
    //    GET_FILEEX_INFO_LEVELS RequestedFileInfo = GetFileExInfoStandard;
    //    WIN32_FILE_ATTRIBUTE_DATA FileAttributes = { };
    //
    //    size_t Result = 0;
    //    size_t Size = 0;
    //
    //    Result = GetFileAttributesExA(Path, RequestedFileInfo, &FileAttributes);
    //
    //    if(Result == 0)
    //    {
    //        DWORD ErrorCode = GetLastError();
    //
    //        LogError("failed to get file attributes.\n", ErrorCode);
    //    }
    //    else
    //    {
    //        LARGE_INTEGER FileSize = { };
    //
    //        FileSize.LowPart = FileAttributes.nFileSizeLow;
    //        FileSize.HighPart = FileAttributes.nFileSizeHigh;
    //
    //        Size = FileSize.QuadPart;
    //    }
    //
    //    return Size;
    //}

    size_t file::GetSizeOnDisk()
    {
        size_t Result = 0;
        size_t Size = 0;

        LARGE_INTEGER Tmp = { };

        Result = GetFileSizeEx(HandleId, &Tmp);

        if(Result == 0)
        {
            DWORD ErrorCode = GetLastError();

            // LogError("failed to get file size.\n", ErrorCode);
        }
        else
        {
            Size = Tmp.QuadPart;
        }

        return Size;
    }

    bool file::SyncInternalBufferWithDisk()
    {
        if(IsOpen && !IsSync)
        {
            size_t SizeOnDisk = GetSizeOnDisk();

            if(SizeOnDisk == 0)
            {
                IsSync = true;

                // nothing loaded into memory from
                // physical storage volumes
                IsLoaded = false;
            }
            else
            {
                size_t BufferSize = SizeOnDisk;
                Buffer.resize(BufferSize);

                DWORD BytesRead = 0;
                DWORD Result = 0;

                Result = ReadFile(HandleId, Buffer.data(), BufferSize, &BytesRead, 0);

                if(Result > 0 && BytesRead == BufferSize)
                {
                    IsSync = true;
                    IsLoaded = true;
                }
                else
                {
                    DWORD ErrorCode = GetLastError();

                    // LogError("failed to sync buffer with disk.", ErrorCode);
                }
            }
        }

        return IsSync;
    }

    bool file::ParseFilename()
    {
        bool Success = true;

        size_t PathLength = AbsPath.size();
        size_t LastDelimPos = 0;

        for(size_t CharIndex = 0; CharIndex < PathLength; CharIndex++)
        {
            if(AbsPath[CharIndex] == '\\')
            {
                LastDelimPos = CharIndex;
            }
        }

        if(LastDelimPos == 0)
        {
            Success = false;

            // malformed system path
            // log this occurrence, maybe fail exit
        }

        if(Success && LastDelimPos < PathLength)
        {
            size_t Offset = LastDelimPos + 1;

            Name.insert(begin(Name),
                        begin(AbsPath) + Offset,
                        end(AbsPath));
        }

        return Success;
    }

    const char* file::BeginBuffer()
    {
        return Buffer.data();
    }

    const char* file::EndBuffer()
    {
        return Buffer.data() + Buffer.size();
    }

    bool file::Open(const char* Path,
                    DWORD DesiredAccess,
                    DWORD ShareAccess,
                    DWORD CreateFlags,
                    DWORD AttributeFlags,
                    LPSECURITY_ATTRIBUTES Security)
    {
        HANDLE FileHandle = nullptr;

        FileHandle = CreateFileA(Path, DesiredAccess, ShareAccess, Security, CreateFlags, AttributeFlags, 0);

        if(FileHandle == INVALID_HANDLE_VALUE)
        {
            DWORD ErrorCode = GetLastError();

            // LogError("CreateFile() failed.\n", ErrorCode);

            return false;
        }

        HandleId = FileHandle;

        DesiredAccess = DesiredAccess;
        ShareAccess = ShareAccess;
        CreateFlags = CreateFlags;
        AttributeFlags = AttributeFlags;

        AbsPath = Path;

        ParseFilename();
        StatFile();

        // SizeOnDisk = GetSizeOnDisk();

        IsDir = false;
        IsOpen = true;

        IsSync = false;
        IsLoaded = false;

        // if(HasReadAccess(DesiredAccess) && !SyncInternalBufferWithDisk())
        {
            // LogError("failed to sync node.\n", -1);

            return false;
        }

        return true;
    }

    bool file::Create(const char* Path, bool OverWrite)
    {
        file* Node = nullptr;

        DWORD DesiredAccess = GENERIC_WRITE | GENERIC_READ;
        DWORD ShareAccess = FILE_SHARE_READ | FILE_SHARE_DELETE;
        SECURITY_ATTRIBUTES SecAttributes = { };
        DWORD CreateFlags = !OverWrite ? CREATE_NEW : CREATE_ALWAYS;
        DWORD AttributeFlags = FILE_ATTRIBUTE_NORMAL;

        if(!Open(Path, DesiredAccess, ShareAccess, CreateFlags, AttributeFlags, 0))
        {
            DWORD ErrorCode = GetLastError();

            // LogError("failed to create file.\n", ErrorCode);

            return false;
        }

        return true;
    }

    bool file::OpenForReading(const char* Path)
    {
        DWORD DesiredAccess = GENERIC_READ;
        DWORD ShareAccess = FILE_SHARE_READ | FILE_SHARE_DELETE;
        SECURITY_ATTRIBUTES SecAttributes = { };
        DWORD CreateFlags = OPEN_EXISTING;
        DWORD AttributeFlags = FILE_ATTRIBUTE_NORMAL;

        if(!Open(Path, DesiredAccess, ShareAccess, CreateFlags, AttributeFlags))
        {
            DWORD ErrorCode = GetLastError();

            // LogError("failed to open file for reading.\n", ErrorCode);

            return false;
        }

        return true;
    }

    bool file::OpenForWriting(const char* Path, bool OverWrite)
    {
        DWORD DesiredAccess = GENERIC_WRITE;
        DWORD ShareAccess = FILE_SHARE_READ | FILE_SHARE_DELETE;
        SECURITY_ATTRIBUTES SecAttributes = { };
        DWORD CreateFlags = !OverWrite ? OPEN_EXISTING : TRUNCATE_EXISTING;
        DWORD AttributeFlags = FILE_ATTRIBUTE_NORMAL;

        if(!Open(Path, DesiredAccess, ShareAccess, CreateFlags, AttributeFlags))
        {
            DWORD ErrorCode = GetLastError();

            // LogError("failed to open file for writing.\n", ErrorCode);

            return false;
        }

        return true;
    }

    bool file::OpenForReadAndWrite(const char* Path, bool OverWrite)
    {
        DWORD DesiredAccess = GENERIC_WRITE | GENERIC_READ;
        DWORD ShareAccess = FILE_SHARE_READ | FILE_SHARE_DELETE;
        SECURITY_ATTRIBUTES SecAttributes = { };
        DWORD CreateFlags = !OverWrite ? OPEN_EXISTING : TRUNCATE_EXISTING;
        DWORD AttributeFlags = FILE_ATTRIBUTE_NORMAL;

        if(!Open(Path, DesiredAccess, ShareAccess, CreateFlags, AttributeFlags))
        {
            DWORD ErrorCode = GetLastError();

            // LogError("failed to open file for reading and writing.\n", ErrorCode);

            return false;
        }

        return true;
    }

    void file::GetDirectoryContents(const char* Path, vector<file*>& Files)
    {
        string _Path(Path);

        if(!_Path.empty())
        {
            _Path.append("\\*.*");

            WIN32_FIND_DATA Result = { };
            HANDLE DirHandle = FindFirstFileA(_Path.c_str(), &Result);

            if(DirHandle == INVALID_HANDLE_VALUE)
            {
                // string Test = GetLastErrorString(GetLastError());

                // error encountered; check severity
                // exit loop and dont process
                return;
            }

            while(DirHandle)
            {
                file* FileNode = new file();

                string AbsPath(Path);

                AbsPath.append("\\");
                AbsPath.append(Result.cFileName);

                Name = Result.cFileName;
                AbsPath = move(AbsPath);

                StatFile();

                Files.push_back(FileNode);

                if(!FindNextFile(DirHandle, &Result))
                {
                    DWORD ErrorCode = GetLastError();

                    if(ErrorCode != ERROR_NO_MORE_FILES)
                    {
                        if(FileNode)
                        {
                            delete FileNode;
                            FileNode = nullptr;
                        }

                        // string ErrorMessage = GetLastErrorString(GetLastError());
                    }

                    break;
                }
            };

            if(DirHandle)
            {
                FindClose(DirHandle);
                DirHandle = 0;
            }
        }
    }

    void file::Close()
    {
        if(HandleId)
        {
            CloseHandle(HandleId);
            HandleId = nullptr;
        }

        Name.clear();
        Name.shrink_to_fit();

        Buffer.clear();
        Buffer.shrink_to_fit();

        IsDir = false;
        IsOpen = true;

        IsSync = false;
        IsLoaded = false;
    }

    bool file::TryToSetASCII()
    {
        // override EncodingAndBOMSet boolean 
        // to ensure its set initially
        EncodingAndBOMSet = false;

        bool Success = true;

        const uint8_t* Bytes = (const uint8_t*)Buffer.data();
        size_t ByteCount = Buffer.size();

        if(ByteCount > 0 && Bytes)
        {
            for(size_t ByteIndex = 0; ByteIndex < ByteCount; ByteIndex++)
            {
                if(Bytes[ByteIndex] < 0 || Bytes[ByteIndex] > 127)
                {
                    Success = false;
                    break;
                }
            }
        }

        if(Success)
        {
            Encoding = FileEncoding(ASCII);

            size_t HighNibbleByteCount = 0;
            size_t LowNibbleByteCount = 0;

            for(size_t ByteIndex = 0;
                ByteIndex < ByteCount;
                ByteIndex++)
            {
                if(HighNibble8(Bytes[ByteIndex]) > 0)
                {
                    HighNibbleByteCount++;
                }
                else if(LowNibble8(Bytes[ByteIndex]) > 0)
                {
                    LowNibbleByteCount++;
                }
            }

            float PercentHighNibble = 0.0f;
            float PercentLowNibble = 0.0f;

            PercentHighNibble = HighNibbleByteCount / (float)ByteCount;
            PercentLowNibble = LowNibbleByteCount / (float)ByteCount;

            if(PercentHighNibble > PercentLowNibble)
            {
                Endianness = FileEndianness(BigEndian);
            }
            else
            {
                Endianness = FileEndianness(LittleEndian);
            }
        }

        return Success;
    }

    bool file::TryToSetUTF8BOM()
    {
        // override EncodingAndBOMSet boolean 
        // to ensure its set initially
        EncodingAndBOMSet = false;

        const uint8_t* Bytes = (const uint8_t*)Buffer.data();
        size_t ByteCount = Buffer.size();

        if(ByteCount > 0 && Bytes)
        {
            const uint32_t Bitmask = (Bytes[2] << 16) |
                (Bytes[1] << 8) |
                Bytes[0];

            const uint32_t UTF8_BOM = (0xBF << 16) |
                (0xBB << 8) |
                0xEF;

            // try to match bytes for utf16 big endian BOM
            if((Bitmask & 0x00FFFFFF) == UTF8_BOM)
            {
                Encoding = FileEncoding(UTF8);
                Endianness = FileEndianness(Unknown);

                // set object state flag
                EncodingAndBOMSet = true;
            }
        }

        if(!EncodingAndBOMSet)
        {
            Encoding = FileEncoding(Unknown);
            Endianness = FileEndianness(Unknown);
        }

        return EncodingAndBOMSet;
    }

    bool file::TryToSetUTF16BOM()
    {
        // override EncodingAndBOMSet boolean 
        // to ensure its set initially
        EncodingAndBOMSet = false;

        const uint8_t* Bytes = (const uint8_t*)Buffer.data();
        size_t ByteCount = Buffer.size();

        if(ByteCount > 0 && Bytes)
        {
            uint32_t Bitmask = *Bytes;

            if(HighByte32(Bitmask) > 0)
            {
                const uint32_t BIG_ENDIAN_UTF16_BOM = (0xBF << 24) |
                    (0xBB << 16);

                // try to match bytes for utf16 big endian BOM
                if((Bitmask & 0xFFFF0000) == BIG_ENDIAN_UTF16_BOM)
                {
                    Encoding = FileEncoding(UTF16);
                    Endianness = FileEndianness(Unknown);

                    // set object state flag
                    EncodingAndBOMSet = true;
                }
            }
            else if(LowByte32(Bitmask) > 0)
            {
                const uint32_t LITTLE_ENDIAN_UTF16_BOM = (0xBF << 8) |
                    0xBB;

                // try to match bytes for utf16 little endian BOM
                if((Bitmask & 0x0000FFFF) == LITTLE_ENDIAN_UTF16_BOM)
                {
                    Encoding = FileEncoding(UTF16);
                    Endianness = FileEndianness(Unknown);

                    // set object state flag
                    EncodingAndBOMSet = true;
                }
            }
        }

        if(!EncodingAndBOMSet)
        {
            Encoding = FileEncoding(Unknown);
            Endianness = FileEndianness(Unknown);
        }

        return EncodingAndBOMSet;
    }

    bool file::TryToSetBOMAndFileEncoding()
    {
        // override EncodingAndBOMSet boolean 
        // to ensure its set initially
        EncodingAndBOMSet = false;

        if(!EncodingAndBOMSet && TryToSetUTF8BOM())
        {
            // log successfull attempt to st BOM
            // and encoding
        }
        else
        {
            // log failed attempt to set utf8 bom
            // set return state
        }

        if(!EncodingAndBOMSet && TryToSetUTF16BOM())
        {
            // log successfull attempt to st BOM
            // and encoding
        }
        else
        {
            // log failed attempt to set utf8 bom
            // set return state
            // Success = false;
        }

        if(!EncodingAndBOMSet && TryToSetASCII())
        {
            // log successfull attempt to st BOM
            // and encoding
        }
        else
        {
            // log failed attempt to set utf8 bom
            // set return state
            // Success = false;
        }

        return EncodingAndBOMSet;
    }
}
