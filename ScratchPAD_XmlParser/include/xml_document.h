#ifndef XML_DOCUMENT_H
#define XML_DOCUMENT_H

#include "xml_markup.h"

namespace scratchpad
{
	enum class xml_decl_attributes
	{
		Version = 0,
		Encoding = 1,
		Standalone = 2,
		Count = 3
	};

	class xml_document
	{
		friend class xml_source;

	public:
		 xml_document();
		~xml_document();

	public:
		void SetDocAttribute(xml_decl_attributes AttributeEnum, string Value);

	private:
		void PushMarkup(string&& Text);
		void PopMarkup();

	private:
		// all the things, just the
		// document markup tree
		xml_markup* Markup;

		// document specific data members
		string		Decl[(int) xml_decl_attributes::Count];
		bool		ParsedDecl;
	};
}

#endif