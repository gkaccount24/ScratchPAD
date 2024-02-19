#ifndef XML_MARKUP_H
#define XML_MARKUP_H

#include <defs.h>

namespace scratchpad
{
	struct xml_markup_attribute
	{
		string Name;
		string Value;
	};

	/***
	***** MARKUP CLASS
	****/

	class xml_markup
	{

	public:
		xml_markup(string StartTag);
		xml_markup(string StartTag,
				   string EndTag);
		~xml_markup();

		/***
		*** STATIC XML MARKUP METHODS
		**/

	public:
		static xml_markup* Create(const char* FirstLexeme,
								  const char* LastLexeme);
		static xml_markup* Create(const char* NameToken,
								  size_t Length);

	public:
		string StartTag;
		string EndTag;
		string Text;

		// parsed/preconfigured markup attributes
		vector<xml_markup_attribute*> Attributes;

		// child markup
		vector<xml_markup*> Children;
	};
}

#endif
