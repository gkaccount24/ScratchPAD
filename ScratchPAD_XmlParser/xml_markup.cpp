#include "./include/xml_markup.h"

namespace scratchpad
{
	/***
	*** XML MARKUP CONSTRUCTORS
	**/

	xml_markup::xml_markup(xml_markup_types MarkupType, string MarkupStartTag):
		Type(MarkupType),
		StartTag(MarkupStartTag),
		EndTag(), Content()
	{

	}

	xml_markup::xml_markup(xml_markup_types MarkupType, string MarkupStartTag, string MarkupEndTag):
		Type(MarkupType),
		StartTag(MarkupStartTag),
		EndTag(MarkupEndTag),
		Content()
	{

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

			Children.clear();
			Children.shrink_to_fit();
		}
	}
}
