#include "source.h"

namespace scratchpad
{
	namespace xml
	{
		source::source(string Path):
			SourceFile(Path),
			SourceFileSize(0)
		{
			/***
			***** primary source file 
			***** constructor
			***/
		}

		source::source():
			SourceFile(),
			SourceFileSize(0)
		{
			/***
			***** default
			***** constructor
			***/
		}

		source::~source()
		{
			if(SourceFile.is_open())
			{
				SourceFile.close();
			}
		}

		inline bool parser::ReadSourceFile(string XMLSourceDiskPath)
		{
			if(!SourceFile.is_open())
			{
				// failed to open source file
				// log failure to open xml source
				SetErrorFileNotOpen(XMLSourceDiskPath.c_str());
				return false;
			}

			SourceFile.seekg(0, std::ios_base::end);
			SourceBuffSize = SourceFile.tellg();
			SourceFile.seekg(0, std::ios_base::beg);

			string TempBuff;

			TempBuff.resize(SourceBuffSize);

			SourceFile.read(TempBuff.data(), 
							TempBuff.size());

			SourceFile.close();

			SourceBuff.str(move(TempBuff));

			return true;
		}
	}
}


