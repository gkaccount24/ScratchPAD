#include "markup.h"

namespace scratchpad
{
	namespace xml
	{
		/***
		*** XML MARKUP CONSTRUCTORS
		**/
		markup::markup(markup_types MarkupType, 
					   string MarkupStartTag):
			Type(MarkupType),
			StartTag(MarkupStartTag),
			EndTag(), Content()
		{

		}

		markup::markup(markup_types MarkupType, 
					   string MarkupStartTag, 
					   string MarkupEndTag):
			Type(MarkupType),
			StartTag(MarkupStartTag),
			EndTag(MarkupEndTag),
			Content()
		{

		}

		markup::~markup()
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
}
