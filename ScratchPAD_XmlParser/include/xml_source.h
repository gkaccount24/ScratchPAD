#ifndef XML_SOURCE_H
#define XML_SOURCE_H

/***
*** MY INCLUDES
***/

/***
*** C++ INCLUDES
***/

#include <string>

namespace scratchpad
{
	using std::string;

	class xml_source
	{

	public:
		xml_source(const char* Path);
		xml_source(xml_source&& RSource) noexcept;

		~xml_source();

	public:
		const char* DiskPath;
		string		Buffer;
	};
}

#endif