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
			void Read(size_t ByteCount);
			void ReadAll();

			bool Open(string SourcePath);
			void Close();

		public:
			/****
			***** SOURCE
			***** FILE PATH
			****/
			string			  Path;

			/****
			***** SOURCE
			***** FILE HANDLE & 
			***** FILE SIZE
			****/
			fstream			  File;
			size_t			  FileSize;

			/****
			***** BUFFER STREAM
			****/
			stringstream Buffer;
			string		 ReadCache;
			
			/****
			***** DIAGNOSTICS
			****/
			xml::diagnostics* Diagnostics;
		};
	}
}

#endif