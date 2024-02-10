#ifndef XML_MARKUP_H
#define XML_MARKUP_H

#include <defs.h>

/***
*** Some different type of 
*** built in markup tags to 
*** read for
*/
enum class xml_builtin_markup_tags
{
	PrologAndTypeDeclTag	 = 0,
	CommentTag				 = 1,
	ProcessingInstructionTag = 2,
	CDataSectionTag			 = 3,
	CharDataTag				 = 4,
	Count					 = 5
};

#define BuiltinMarkupTagTypeEnum(MarkupTag)(xml_builtin_markup_tags::MarkupTag)
#define BuiltinMarkupTagIndex(MarkupTag)((int) MarkupTag)

struct xml_markup_attribute
{
	string ExpectedBytes;
	string ActualBytes;
	string Value;
};

#define MarkupAttributeEnum(Attribute)(xml_builtin_markup_tag_attributes::Attribute)
#define MarkupAttributeIndex(Attribute)((int) MarkupAttributeEnum(Attribute))
#define MarkupAttribute(MarkupTag, Attribute)(BuiltinMarkupTag(MarkupTag)->Attributes[BuiltinMarkupTagAttributeIndex(Attribute)])

/***
***** MARKUP STRUCT
*****
****/

struct xml_markup
{
	// tag type id
	xml_builtin_markup_tags Type;

	// starting tag lexeme
	string FirstLexeme;
	string LastLexeme;

	bool DocumentDeclaration;

	// parsed/preconfigured markup attributes
	vector<xml_markup_attribute*> Attributes;

	// child markup
	vector<xml_markup*> Children;

	xml_markup(xml_builtin_markup_tags TagType, 
			   const char* StartLexeme, 
			   const char* EndLexeme);

	~xml_markup();

	/***
	*** STATIC XML MARKUP METHODS
	**/

	static xml_markup* Create(xml_builtin_markup_tags TagType, 
							  const char* StartLexeme,
							  const char* EndLexeme);
};

/***
*** STATIC XML MARKUP DATA
**/
inline static const xml_markup* GetBuiltinMarkupTags()
{
	static xml_markup_attribute Attributes[]
	{
		xml_markup_attribute  { "version"    },
		xml_markup_attribute  { "encoding"   },
		xml_markup_attribute  { "standalone" },
	};

	return nullptr;
}

#endif
