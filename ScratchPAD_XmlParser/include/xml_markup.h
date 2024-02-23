#ifndef XML_MARKUP_H
#define XML_MARKUP_H

#include <defs.h>

namespace scratchpad
{
	enum class xml_markup_types
	{
		DocDeclTag,
		DocTypeTag,
		CommentTag,
		ElementType,
		EntityDecl,
	};

	struct xml_markup_attribute
	{
		string Name;
		string Value;
	};

	/***
	***** MARKUP CLASS
	****/

#define DATA public

	class xml_markup
	{

	public:
		xml_markup(xml_markup_types MarkupType, 
				   string StartTag);
		xml_markup(xml_markup_types MarkupType,
				   string StartTag,
				   string EndTag);

		~xml_markup();

	DATA:
		xml_markup_types Type;
		string			 StartTag;
		string			 EndTag;
		string			 Text;

		vector<xml_markup_attribute*> Attributes;
		vector<xml_markup*>			  Children;
	};
}

#endif
