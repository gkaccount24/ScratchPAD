#ifndef XML_SOURCE_H
#define XML_SOURCE_H

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

			 source(string Path);

		public:

			 source();
			~source();

		public:
			/****
			***** SOURCE
			***** FILE HANDLE
			****/
			fstream		 SourceFile;
			size_t		 SourceFileSize;

		};
	}
}

#endif