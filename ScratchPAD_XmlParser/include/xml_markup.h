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

struct xml_markup_attribute
{
	string Name;
	string Value;
};

static xml_markup_attribute* CreateAttribute(const char* AttributeName)
{
	return new xml_markup_attribute { AttributeName };
}

/***
***** MARKUP STRUCT
*****
****/

struct xml_markup
{
	string StartTag;
	string EndTag;
	string Text;

	// parsed/preconfigured markup attributes
	vector<xml_markup_attribute*> Attributes;

	// child markup
	vector<xml_markup*> Children;

	xml_markup(const char* StartLexeme, 
			   const char* EndLexeme);

	xml_markup(string&& NameToken);

	~xml_markup();

	/***
	*** STATIC XML MARKUP METHODS
	**/

	static xml_markup* Create(const char* FirstLexeme, 
							  const char* LastLexeme);

	static xml_markup* Create(const char* NameToken, 
							  size_t Length);

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
