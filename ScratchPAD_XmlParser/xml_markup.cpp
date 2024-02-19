#include "./include/xml_markup.h"

namespace scratchpad
{
	/****
	*****
	***** XML MARKUP HELPER MACROS
	*****
	*****/

	/***
	*** XML MARKUP CONSTRUCTORS
	**/
	xml_markup::xml_markup(string MarkupStartTag):
		StartTag(move(MarkupStartTag)),
		EndTag()
	{

	}

	xml_markup::xml_markup(string MarkupStartTag,
						   string MarkupEndTag):
		StartTag(move(MarkupStartTag)),
		EndTag(move(MarkupEndTag))
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
		}
	}
}
