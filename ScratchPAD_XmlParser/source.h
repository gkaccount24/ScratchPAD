#ifndef XML_SOURCE_H
#define XML_SOURCE_H

#include "diagnostics.h"

#include <string>
#include <fstream>

using std::string;
using std::fstream;

namespace scratchpad
{
	namespace xml
	{
		class source
		{

		public:

			 source(string SourcePath);

		public:

			 source();
			~source();

		public:

			size_t Read(string& FileBuffer, size_t ByteCount);
			size_t ReadAll(string& FileBuffer);

			bool Open(string SourcePath);
			void Close();

		public:
			/****
			***** SOURCE
			***** FILE HANDLE
			****/
			string			  Path;
			fstream			  File;
			size_t			  FileSize;
			xml::diagnostics* Diagnostics;
		};
	}
}

#endif