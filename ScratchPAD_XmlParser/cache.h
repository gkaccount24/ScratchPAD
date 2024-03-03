#ifndef XML_PARSER_CACHE_H
#define XML_PARSER_CACHE_H

#include "document.h"

// we can lex this character in these 
// scenarios
//if(StateMatches(parsing_states::ParsingContent) ||
//   StateMatches(parsing_states::ParsingStartTag))
//{
//	if(StateMatches(parsing_states::ParsingContent))
//	{
//		SwitchState(parsing_states::ParsingStartTag);
//	}	

//	if(Markup.empty())
//	{
//		if(TryToParseDeclStart())
//		{
//			PushMarkup(markup_types::DocDeclTag, XMLDeclStartTag.data());

//			break;
//		}
//	}
//	else
//	{
//		if(TryToParseTypeStart())
//		{
//			PushMarkup(markup_types::DocTypeTag, XMLDocTypeStartTag.data());

//			break;
//		}
//		else if(TryToParseCommentStart())
//		{
//			PushMarkup(markup_types::CommentTag, CommentStartTag.data());

//			break;
//		}
//		else
//		{
//			// advance input buffer
//			// fewer bytes to read;
//			Buffer->sbumpc();

//			if(TryToParseNameToken('>'))
//			{
//				Buffer->sbumpc();

//				PushMarkup(markup_types::UserTag, move(WriteBuffer));
//				SwitchState(parsing_states::ParsingContent);
//			}
//			else
//			{
//				if(!Error)
//				{
//					PushMarkup(markup_types::UserTag, move(WriteBuffer));
//					SwitchState(parsing_states::ParsingAtts);
//				}
//			}
//		}
//	}
//}

namespace scratchpad
{
	namespace xml
	{
		class document_cache
		{

		public:

			 document_cache() = default;
			~document_cache() = default;

		public:

			void Put(xml::document* Document);



		};
	}
}

#endif
