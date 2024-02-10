#ifndef XML_COMMENT_H
#define XML_COMMENT_H

#include <defs.h>

struct xml_comment
{
	size_t ByteOffset;
	size_t LineNumber;
	size_t LineSpan;
	size_t Column;
	size_t Row;
};

#endif
