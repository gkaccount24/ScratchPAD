#ifndef XML_MARKUP_H
#define XML_MARKUP_H

#include <vector>
#include <string>
#include <sstream>

using std::string;
using std::vector;

using std::stringstream;

namespace scratchpad
{
	namespace xml
	{
		enum class markup_types 
		{
			DocDeclTag,
			DocTypeTag,
			CommentTag,
			ElementType,
			EntityDecl,
			GenericTag
		};

		struct markup_attribute
		{
			string Name;
			string Value;
		};

		/***
		***** MARKUP CLASS
		****/

	#define DATA public

		class markup
		{

		public:
			 markup(markup_types MarkupType, string StartTag);
			 markup(markup_types MarkupType, string StartTag, string EndTag);
			~markup();

		DATA:
			markup_types Type;
			string			 StartTag;
			string			 EndTag;
			string			 Content;

			vector<markup_attribute*> Attributes;
			vector<markup*>			  Children;
		};
	}
}

#endif

