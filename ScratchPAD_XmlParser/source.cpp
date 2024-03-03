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
			Buffer(EMPTY_BUFFER_STRING),
			Diagnostics(xml::diagnostics::ParserDiagnostics())
		{
			/***
			***** primary source file 
			***** constructor
			***/
			Diagnostics->WriteInfoLog("creating xml source...");

			if(File.is_open())
			{
				File.seekg(0, std::ios_base::end);
				FileSize = File.tellg();
				File.seekg(0, std::ios_base::beg);
			}
		}

		source::source():
			Path(EMPTY_BUFFER_STRING),
			File(EMPTY_BUFFER_STRING),
			FileSize(0),
			Buffer(EMPTY_BUFFER_STRING),
			Diagnostics(xml::diagnostics::ParserDiagnostics())
		{
			/***
			***** default
			***** constructor
			***/
			Diagnostics->WriteInfoLog("creating xml source...");
		}

		source::~source()
		{
			Diagnostics->WriteInfoLog("destroying xml source...");

			if(File.is_open())
			{
				File.close();
			}
		}

		bool source::Open(string SourcePath)
		{
			if(!File.is_open())
			{
				File.open(Path);

				if(!File.is_open())
				{
					Diagnostics->WriteErrorLog("failed to open xml source file...");

					return false;
				}
			}

			Diagnostics->WriteInfoLog("xml source is opened...");

			Path = SourcePath;

			File.seekg(0, std::ios_base::end);
			FileSize = File.tellg();
			File.seekg(0, std::ios_base::beg);

			return true;
		}

		void source::Close()
		{
			Diagnostics->WriteInfoLog("closing xml source file...");

			if(File.is_open())
			{
				File.close();
			}

			Path.clear();
		}

		void source::Read(size_t ByteCount)
		{
			if(ByteCount > 0)
			{
				ReadCache.resize(ByteCount);

				File.read(ReadCache.data(), 
						  ByteCount);
			}
		}

		void source::ReadAll()
		{
			if(!File.is_open())
			{
				// failed to open source file
				// log failure to open xml source
				Diagnostics->SetErrorFileNotOpen(Path.c_str());

				return;
			}
	
			if(FileSize > 0)
			{
				Read(FileSize);

				if(!ReadCache.empty())
				{
					Buffer.str(move(ReadCache));
				}
			}
		}
	}
}


