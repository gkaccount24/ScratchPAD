#include "./include/xml_markup.h"

/****
*****
***** XML MARKUP HELPER MACROS
*****
*****/


#define MarkupTagTypeIndex(MarkupTag)((int)MarkupTag)
#define MAX_XML_TAG_ATTRIBUTE_COUNT 128

xml_markup::xml_markup(xml_builtin_markup_tags MarkupTagType,
					   const char* MarkupFirstLexeme,
					   const char* MarkupLastLexeme)
{
	Type = MarkupTagType;
	FirstLexeme = MarkupFirstLexeme;
	LastLexeme = MarkupLastLexeme;
}

xml_markup::~xml_markup()
{
	if(!Attributes.empty())
	{
		Attributes.clear();
		Attributes.shrink_to_fit();
	}

	if(!Children.empty())
	{
		size_t ChildCount = Children.size();

		for(size_t Index = 0; Index < ChildCount; Index++)
		{
			if(Children[Index])
			{
				delete Children[Index];
				Children[Index] = nullptr;
			}
		}
	}
}
