#include "./include/stream_io.h"

namespace scratchpad
{
    stream_io::stream_io(io_device& StreamIODevice):
        IODevice(StreamIODevice)
    {
    }

    stream_io::~stream_io()
    {
        
    }

    void stream_io::Write(const char* Bytes, size_t Count)
    {
        
    }

    size_t stream_io::Read(char* ReceiveBuf, size_t Count)
    {

        return 0;
    }
}