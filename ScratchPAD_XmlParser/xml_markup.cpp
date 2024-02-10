#include "./include/xml_markup.h"

/****
*****
***** XML MARKUP HELPER MACROS
*****
*****/

/***
*** XML MARKUP 
*** STATIC METHODS
**/

xml_markup* xml_markup::Create(const char* StartTag,
							   const char* EndTag)
{
	return new xml_markup(StartTag, EndTag);
}

/***
*** XML MARKUP CONSTRUCTORS
**/

xml_markup::xml_markup(const char* MarkupStartTag,
					   const char* MarkupEndTag)
{
	StartTag = MarkupStartTag;
	EndTag = MarkupEndTag;
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
