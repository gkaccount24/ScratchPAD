#include "./include/win32_io.h"

using std::cout;
using std::endl;

enum logging_label: unsigned int
{
    Error = 0, 
    Notice = 1,
    Warning = 2, 
    Info = 3,
    LabelCount = 4
};

static void ConsoleLog(HANDLE Device, logging_label Label, const char* Message)
{
    const char* LabelText[logging_label::LabelCount] = 
    {
        "[ERROR] ",
        "[NOTICE] ",
        "[WARNING] ",
        "[INFO] "
    };

    string _Message(Message);

    if(!_Message.empty())
    {
        string _OutputBuffer(LabelText[Label]);
        _OutputBuffer.append(Message);

        DWORD CharsWritten = 0;

        WriteConsole(Device, _OutputBuffer.c_str(), _OutputBuffer.size(), &CharsWritten, 0);
    }
}

void ConsoleNotice(const char* Message)
{
    HANDLE Device = GetStdHandle(STD_OUTPUT_HANDLE);

    ConsoleLog(Device, logging_label::Notice, Message);
}

void ConsoleWarning(const char* Message)
{
    HANDLE Device = GetStdHandle(STD_OUTPUT_HANDLE);

    ConsoleLog(Device, logging_label::Warning, Message);
}

void ConsoleError(const char* Message)
{
    HANDLE Device = GetStdHandle(STD_ERROR_HANDLE);

    ConsoleLog(Device, logging_label::Error, Message);
}

static string GetLastErrorString(DWORD ErrorCode)
{
    DWORD MessageFlags = FORMAT_MESSAGE_ALLOCATE_BUFFER | 
                         FORMAT_MESSAGE_FROM_SYSTEM;

    CHAR* MessageBuffer = 0;
    DWORD Result = 0;

    Result = FormatMessageA(MessageFlags, 0, ErrorCode, 0, 
                           (LPSTR) &MessageBuffer, 0, 0);

    string ErrorString;

    if(Result != 0)
    {
        ErrorString = MessageBuffer;

        LocalFree(MessageBuffer);
    }

    return ErrorString;
}

static void LogError(const char* Message, DWORD ErrorCode)
{
    /*
     * ERROR_FILE_NOT_FOUND:
     * ERROR_PATH_NOT_FOUND:
     * ERROR_ACCESS_DENIED:
     * ERROR_TOO_MANY_OPEN_FILES:
     * ERROR_INVALID_HANDLE:
     * ERROR_HANDLE_EOF:
     * ERROR_IO_PENDING:
     * ERROR_LOCK_VIOLATION:
     * ERROR_NOT_SUPPORTED:
     * ERROR_USER_MAPPED_FILE:
    */

    string ErrorMessage = GetLastErrorString(ErrorCode);

    ConsoleError(Message);
    ConsoleError(ErrorMessage.c_str());
}

void StatFile(file* Node)
{
    GET_FILEEX_INFO_LEVELS RequestedFileInfo = GetFileExInfoStandard;

    size_t Result = 0;
    size_t Size = 0;

    Result = GetFileAttributesExA(Node->_AbsPath.c_str(), RequestedFileInfo, &Node->Stat);

    if(Result == 0)
    {
        DWORD ErrorCode = GetLastError();

        LogError("failed to get file attributes.\n", ErrorCode);
    }
    else
    {
        LARGE_INTEGER FileSize = { };

        FileSize.LowPart = Node->Stat.nFileSizeLow;
        FileSize.HighPart = Node->Stat.nFileSizeHigh;

        Size = FileSize.QuadPart;
        Node->_SizeOnDisk = Size;
    }
}

size_t GetSizeOnDisk(const char* Path)
{
    GET_FILEEX_INFO_LEVELS RequestedFileInfo = GetFileExInfoStandard;
    WIN32_FILE_ATTRIBUTE_DATA FileAttributes = { };

    size_t Result = 0;
    size_t Size = 0;

    Result = GetFileAttributesExA(Path, RequestedFileInfo, &FileAttributes);

    if(Result == 0)
    {
        DWORD ErrorCode = GetLastError();

        LogError("failed to get file attributes.\n", ErrorCode);
    }
    else
    {
        LARGE_INTEGER FileSize = { };

        FileSize.LowPart = FileAttributes.nFileSizeLow;
        FileSize.HighPart = FileAttributes.nFileSizeHigh;

        Size = FileSize.QuadPart;
    }

    return Size;
}


size_t GetSizeOnDisk(file* Node)
{
    size_t Result = 0;
    size_t Size = 0;

    LARGE_INTEGER Tmp = { };

    Result = GetFileSizeEx(Node->_Id, &Tmp);

    if(Result == 0)
    {
        DWORD ErrorCode = GetLastError();
        
        LogError("failed to get file size.\n", ErrorCode);
    }
    else
    {
        Size = Tmp.QuadPart;
    }

    return Size;
}

bool SyncInternalBufferWithDisk(file* Node)
{
    if(Node->_IsOpen && !Node->_IsSync)
    {
        size_t SizeOnDisk = GetSizeOnDisk(Node);

        if(SizeOnDisk == 0)
        {
            Node->_IsSync = true;

            // nothing loaded into memory from
            // physical storage volumes
            Node->_IsLoaded = false;
        }
        else
        {
            size_t BufferSize = SizeOnDisk;
            Node->_Buffer.resize(BufferSize);

            DWORD BytesRead = 0;
            DWORD Result = 0;

            Result = ReadFile(Node->_Id, Node->_Buffer.data(), BufferSize, &BytesRead, 0);

            if(Result > 0 && BytesRead == BufferSize)
            {
                Node->_IsSync = true;
                Node->_IsLoaded = true;
            }
            else
            {
                DWORD ErrorCode = GetLastError();
                
                LogError("failed to sync buffer with disk.", ErrorCode);
            }
        }
    }

    return Node->_IsSync;
}

#define HasReadAccess(Access)  (((Access) & GENERIC_READ)  > 0)
#define HasWriteAccess(Access) (((Access) & GENERIC_WRITE) > 0)

bool ParseFilename(file* File)
{
    bool Success = true;

    size_t PathLength = File->_AbsPath.size();
    size_t LastDelimPos = 0;

    for(size_t CharIndex = 0; CharIndex < PathLength; CharIndex++)
    {
        if(File->_AbsPath[CharIndex] == '\\')
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

        File->_Name.insert(begin(File->_Name), 
                           begin(File->_AbsPath) + Offset, 
                           end(File->_AbsPath));
    }

    return Success;
}

file* Open(const char* Path, 
              DWORD DesiredAccess, 
              DWORD ShareAccess, 
              DWORD CreateFlags, 
              DWORD AttributeFlags, 
              LPSECURITY_ATTRIBUTES Security)
{
    file* Node = nullptr;

    HANDLE FileHandle = nullptr;

    FileHandle = CreateFileA(Path, DesiredAccess, ShareAccess, Security, CreateFlags, AttributeFlags, 0);

    if(FileHandle == INVALID_HANDLE_VALUE)
    {
        DWORD ErrorCode = GetLastError();

        LogError("CreateFile() failed.\n", ErrorCode);
    }
    else
    {
        Node = new file { };

        Node->_Id = FileHandle;

        Node->_DesiredAccess = DesiredAccess;
        Node->_ShareAccess = ShareAccess;
        Node->_CreateFlags = CreateFlags;
        Node->_AttributeFlags = AttributeFlags;

        Node->_AbsPath = Path;

        ParseFilename(Node);

        Node->_SizeOnDisk = GetSizeOnDisk(Path);

        Node->_IsDir = false;
        Node->_IsOpen = true;

        Node->_IsSync = false;
        Node->_IsLoaded = false;

        if(HasReadAccess(DesiredAccess) && !SyncInternalBufferWithDisk(Node))
        {
            delete Node;
            Node = nullptr;

            LogError("failed to sync node.\n", -1);
        }
        else
        {
			if(!TryToSetBOMAndFileEncoding(Node))
			{
				// log failed attempt to detect/set bom 
				// and file encoding/endianness

			}
			else
			{
				// log successful attempt to detect/set bom 
				// and file encoding/endianness
			}
        }
    }

    return Node;
}

file* Create(const char* Path, bool OverWrite)
{
    file* Node = nullptr;

    DWORD DesiredAccess = GENERIC_WRITE | GENERIC_READ;
    DWORD ShareAccess = FILE_SHARE_READ | FILE_SHARE_DELETE;
    SECURITY_ATTRIBUTES SecAttributes = { };
    DWORD CreateFlags = !OverWrite ? CREATE_NEW : CREATE_ALWAYS;
    DWORD AttributeFlags = FILE_ATTRIBUTE_NORMAL;

    Node = Open(Path, DesiredAccess, ShareAccess, CreateFlags, AttributeFlags, 0);

    if(!Node)
    {
        DWORD ErrorCode = GetLastError();

        LogError("failed to create file.\n", ErrorCode);
    }

    return Node;
}

file* OpenForReading(const char* Path)
{
    DWORD DesiredAccess = GENERIC_READ;
    DWORD ShareAccess = FILE_SHARE_READ | FILE_SHARE_DELETE;
    SECURITY_ATTRIBUTES SecAttributes = { };
    DWORD CreateFlags = OPEN_EXISTING;
    DWORD AttributeFlags = FILE_ATTRIBUTE_NORMAL;

    file* Node = Open(Path, DesiredAccess, ShareAccess, CreateFlags, AttributeFlags);

	if(!Node)
    {
        DWORD ErrorCode = GetLastError();

        LogError("failed to open file for reading.\n", ErrorCode);
    }

    return Node;
}

file* OpenForWriting(const char* Path, bool OverWrite)
{
    DWORD DesiredAccess = GENERIC_WRITE;
    DWORD ShareAccess = FILE_SHARE_READ | FILE_SHARE_DELETE;
    SECURITY_ATTRIBUTES SecAttributes = { };
    DWORD CreateFlags = !OverWrite ? OPEN_EXISTING : TRUNCATE_EXISTING;
    DWORD AttributeFlags = FILE_ATTRIBUTE_NORMAL;

    file* Node = Open(Path, DesiredAccess, ShareAccess, CreateFlags, AttributeFlags);

    if(!Node)
    {
        DWORD ErrorCode = GetLastError();

        LogError("failed to open file for writing.\n", ErrorCode);
    }

    return Node;
}

file* OpenForReadAndWrite(const char* Path, bool OverWrite)
{
    DWORD DesiredAccess = GENERIC_WRITE | GENERIC_READ;
    DWORD ShareAccess = FILE_SHARE_READ | FILE_SHARE_DELETE;
    SECURITY_ATTRIBUTES SecAttributes = { };
    DWORD CreateFlags = !OverWrite ? OPEN_EXISTING : TRUNCATE_EXISTING;
    DWORD AttributeFlags = FILE_ATTRIBUTE_NORMAL;

    file* Node = Open(Path, DesiredAccess, ShareAccess, CreateFlags, AttributeFlags);

    if(!Node)
    {
        DWORD ErrorCode = GetLastError();

        LogError("failed to open file for reading and writing.\n", ErrorCode);
    }

    return Node;
}

void GetDirectoryContents(const char* Path, vector<file*>& Files)
{
    string _Path(Path);

    if(!_Path.empty())
    {
        _Path.append("\\*.*");

        WIN32_FIND_DATA Result = { };
        HANDLE DirHandle = FindFirstFileA(_Path.c_str(), &Result);

        if(DirHandle == INVALID_HANDLE_VALUE)
        {
            string Test = GetLastErrorString(GetLastError());
            string Test2 = Test;

            // error encountered; check severity
            // exit loop and dont process
            return;
        }

        while(DirHandle)
        {
            file* FileNode = new file { };

            string AbsPath(Path);

            AbsPath.append("\\");
            AbsPath.append(Result.cFileName);

            FileNode->_Name = Result.cFileName;
            FileNode->_AbsPath = move(AbsPath);

            StatFile(FileNode);

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

                    string ErrorMessage = GetLastErrorString(GetLastError());
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

void CloseFile(file* File)
{
    if(!File)
    {
        return;
    }

    CloseHandle(File->_Id);

	delete File;
    File = nullptr;
}

bool TryToSetASCII(file* File)
{
    // override EncodingAndBOMSet boolean 
    // to ensure its set initially
    File->EncodingAndBOMSet = false;

    bool Success = true;
    
    const uint8_t* Bytes = (const uint8_t*) File->_Buffer.data();
    size_t ByteCount = File->_Buffer.size();

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
        File->Encoding = FileEncoding(ASCIIEncoding);

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

        PercentHighNibble = HighNibbleByteCount / (float) ByteCount;
        PercentLowNibble = LowNibbleByteCount / (float) ByteCount;

        if(PercentHighNibble > PercentLowNibble)
        {
            File->Endianness = FileEndianness(BigEndian);
        }
        else
        {
            File->Endianness = FileEndianness(LittleEndian);
        }
    }

    return Success;
}

bool TryToSetUTF8BOM(file* File)
{
    // override EncodingAndBOMSet boolean 
    // to ensure its set initially
    File->EncodingAndBOMSet = false;

	const uint8_t* Bytes = (const uint8_t*) File->_Buffer.data();
	size_t ByteCount = File->_Buffer.size();

	if(ByteCount > 0 && Bytes)
	{
		const uint32_t Bitmask = (Bytes[2] << 16) | 
                                 (Bytes[1] << 8) | 
                                  Bytes[0];

		const uint32_t UTF8_BOM = (0xBF << 16) | 
								  (0xBB << 8) | 
								   0xEF;

		// try to match bytes for utf16 big endian BOM
		if((Bitmask & 0x00FFFFFF) ==  UTF8_BOM)
		{
			File->Encoding = FileEncoding(UTF8Encoding);
            File->BOMEndianness = FileBOMEndianness(LittleEndian);
		    File->Endianness = FileEndianness(Unknown);

            // set object state flag
    	    File->EncodingAndBOMSet = true;
		}
	}

    if(!File->EncodingAndBOMSet)
    {
		File->Encoding = FileEncoding(UnknownEncoding);
		File->BOMEndianness = FileBOMEndianness(Unknown);
		File->Endianness = FileEndianness(Unknown);
    }

    return File->EncodingAndBOMSet;
}

bool TryToSetUTF16BOM(file* File)
{
    // override EncodingAndBOMSet boolean 
    // to ensure its set initially
    File->EncodingAndBOMSet = false;

	const uint8_t* Bytes = (const uint8_t*) File->_Buffer.data();
	size_t ByteCount = File->_Buffer.size();

	if(ByteCount > 0 && Bytes)
	{
		uint32_t Bitmask = *Bytes;

        if(HighByte32(Bitmask) > 0)
        {
            const uint32_t BIG_ENDIAN_UTF16_BOM = (0xBF << 24) |
                                                  (0xBB << 16);

            // try to match bytes for utf16 big endian BOM
            if((Bitmask & 0xFFFF0000) ==  BIG_ENDIAN_UTF16_BOM)
            {
                File->Encoding = FileEncoding(UTF16Encoding);
                File->BOMEndianness = FileBOMEndianness(BigEndian);
		        File->Endianness = FileEndianness(Unknown);

				// set object state flag
				File->EncodingAndBOMSet = true;
            }
        }
        else if(LowByte32(Bitmask) > 0)
        {
            const uint32_t LITTLE_ENDIAN_UTF16_BOM = (0xBF << 8) |
                                                      0xBB;

            // try to match bytes for utf16 little endian BOM
            if((Bitmask & 0x0000FFFF) == LITTLE_ENDIAN_UTF16_BOM)
            {
                File->Encoding = FileEncoding(UTF16Encoding);
                File->BOMEndianness = FileBOMEndianness(LittleEndian);
		        File->Endianness = FileEndianness(Unknown);

                // set object state flag
				File->EncodingAndBOMSet = true;
            }
        }
    }

    if(!File->EncodingAndBOMSet)
    {
		File->Encoding = FileEncoding(UnknownEncoding);
		File->BOMEndianness = FileBOMEndianness(Unknown);
		File->Endianness = FileEndianness(Unknown);
    }

    return File->EncodingAndBOMSet;
}

bool TryToSetBOMAndFileEncoding(file* File)
{
    // override EncodingAndBOMSet boolean 
    // to ensure its set initially
    File->EncodingAndBOMSet = false;

    if(!File->EncodingAndBOMSet && TryToSetUTF8BOM(File))
	{
        // log successfull attempt to st BOM
        // and encoding
	}
    else
    {
        // log failed attempt to set utf8 bom
		// set return state
    }

    if(!File->EncodingAndBOMSet && TryToSetUTF16BOM(File))
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

    if(!File->EncodingAndBOMSet && TryToSetASCII(File))
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

    return File->EncodingAndBOMSet;
}
