#ifndef XML_WRITER_H
#define XML_WRITER_H

#include <string>

using std::string;

namespace scratchpad
{
	namespace xml
	{
		class writer
		{

		public:

			 writer() = default;
			~writer() = default;

		public:

			void WrapStartTag(string& TagSource);
			void WrapEndTag(string& TagSource, bool Shortened);

		};
	}
}

#endif
