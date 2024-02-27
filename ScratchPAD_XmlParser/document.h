#ifndef XML_DOCUMENT_H
#define XML_DOCUMENT_H

#include "markup.h"

#include <string>

using std::string;

namespace scratchpad
{
	namespace xml
	{
		enum class xml_decl_attributes
		{
			Version = 0,
			Encoding = 1,
			Standalone = 2,
			Count = 3
		};

		class document
		{

		public:

			 document() = default;
			~document() = default;

			void SetDocAttribute(xml_decl_attributes AttributeEnum, string Value);

		public:
			/***
			***** document specific data members
			****/
			string		Decl[(int) xml_decl_attributes::Count];
			bool		ParsedDecl;

			markup*		Markup;
		};
	}
}

#endif
