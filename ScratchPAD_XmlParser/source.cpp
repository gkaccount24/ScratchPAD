#include "source.h"

#define EMPTY_BUFFER_STRING ""

namespace scratchpad
{
	namespace xml
	{
		source::source(string SourcePath):
			Path(SourcePath),
			File(SourcePath),
			FileSize(0),
			Diagnostics(xml::diagnostics::ParserDiagnostics())
		{
			/***
			***** primary source file 
			***** constructor
			***/
		}

		source::source():
			Path(EMPTY_BUFFER_STRING),
			File(EMPTY_BUFFER_STRING),
			FileSize(0),
			Diagnostics(xml::diagnostics::ParserDiagnostics())
		{
			/***
			***** default
			***** constructor
			***/
		}

		source::~source()
		{
			if(File.is_open())
			{
				File.close();
			}
		}


		bool source::Open(string SourcePath)
		{
			if(File.is_open())
			{
				File.close();

				// reset the path
				Path = SourcePath;
			}

			File.open(Path);

			return File.is_open();
		}

		void source::Close()
		{
			Path.clear();

			if(File.is_open())
			{
				File.close();
			}
		}

		inline size_t source::Read(string& FileBuffer, size_t ByteCount)
		{
			if(ByteCount > 0)
			{
				FileBuffer.resize(ByteCount);

				// read into resized buffer
				return File.readsome(FileBuffer.data(), ByteCount);
			}

			return 0;
		}

		inline size_t source::ReadAll(string& FileBuffer)
		{
			if(!File.is_open())
			{
				// failed to open source file
				// log failure to open xml source
				Diagnostics->SetErrorFileNotOpen(Path.c_str());

				return 0;
			}

			File.seekg(0, std::ios_base::end);
			FileSize = File.tellg();
			File.seekg(0, std::ios_base::beg);

			size_t BytesRead = 0;
			BytesRead = Read(FileBuffer, FileSize);

			File.close();

			return BytesRead;
		}
	}
}


