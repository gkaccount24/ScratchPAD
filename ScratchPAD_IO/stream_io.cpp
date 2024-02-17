#include "./include/stream_io.h"

namespace scratchpad
{
	stream_io::stream_io(io_stringbuffer&& StreamBuffer):
        Buffer(move(StreamBuffer)),
        BeginStream(begin(Buffer.Data)),
        EndStream(end(Buffer.Data)),
        Cursor(begin(Buffer.Data)),
        WSSkipped(0), BytesRead(0),
        Row(0), Col(0), LineCount(0),
        ByteOffset(0), BufferSize(0),
        BytesAvailable(0)
    {
    }

    stream_io::~stream_io()
    {
        
    }

    size_t stream_io::Write(const char* Bytes, size_t Count)
	{
        size_t LastSize = Buffer.Data.size();
        Buffer.Data.append(Bytes, Count);

        if((Buffer.Data.size() - LastSize) != Count) 
        {
            // io error
            // maybe log occurence
        }

		return Count;
	}

    size_t stream_io::Read(char* ReceiveBuf, size_t Count)
    {
        return Buffer.Read(ReceiveBuf, Count);
    }
}