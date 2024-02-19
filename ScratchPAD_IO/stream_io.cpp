#include "./include/stream_io.h"

//namespace scratchpad
//{
//    stream_io::stream_io(fstream&& SourceFile):
//        Stream(move(SourceFile)),
//        WSSkipped(0), BytesRead(0),
//        BytesWritten(0), Row(0), Col(0),
//        LineCount(0)
//    {
//
//    }
//
//    stream_io::~stream_io()
//    {
//        // Bytes.clear();
//        // Bytes.shrink_to_fit();
//    }
//
//    bool stream_io::IsNL()
//    {
//        return (Buf.sgetc() == '\r' ||
//                Buf.sgetc() == '\n');
//    }
//
//    bool stream_io::IsWS()
//    {
//        return (Buf.sgetc() == ' '  ||
//		        Buf.sgetc() == '\t' ||
//                Buf.sgetc() == '\r' || 
//                Buf.sgetc() == '\n');
//    }
//
//    void stream_io::TrimWS()
//    {
//        WSSkipped = 0;
//
//        while(Buf.in_avail() > 0 && IsWS())
//        {
//            if(IsNL())
//            {
//                LineCount++;
//                Row++;
//            }
//
//            WSSkipped++;
//
//            // advance input buffer
//            // less bytes to read after a bump
//            Buf.sbumpc();
//        }
//    }
//
//    size_t stream_io::Write(const char* Bytes, size_t Count)
//	{
//        BytesWritten = Buf.sputn(Bytes, Count);
//
//        if(BytesWritten != Count)
//        {
//            // didn't write all bytes
//            // in trasmit buffer
//            // report issue
//        }
//
//		return Count;
//	}
//
//    size_t stream_io::Read(char* ReceiveBuf, size_t Count)
//    {
//        if(Buf.in_avail() < Count)
//        {
//            Count = Buf.in_avail();
//        }
//
//        BytesRead = Buf.sgetn(ReceiveBuf, Count);
//
//        if(BytesRead != Count)
//        {
//            // didn't read/receive all bytes
//            // to be written to be copied into
//            // our buffer, report issue;
//        }
//
//        return BytesRead;
//    }
//
//    size_t stream_io::ReadUntil(char* ReceiveBuf, size_t ReceiveBufSize, char Delim)
//    {
//        BytesRead = 0;
//
//        while(Buf.in_avail() > 0 && Buf.sgetc() != Delim)
//        {
//            BytesRead++;
//
//            // advance input buffer
//            // less bytes to read
//            Buf.sbumpc();
//        }
//
//        return BytesRead;
//    }
//
//    size_t stream_io::ReadUntilNL(char* ReceiveBuf, size_t ReceiveBufSize)
//    {
//        BytesRead = 0;
//
//        while(Buf.in_avail() > 0 && !IsNL())
//        {
//            BytesRead++;
//
//            // advance input buffer
//            // less bytes to read
//            Buf.sbumpc();
//        }
//
//        return BytesRead;
//    }
//}