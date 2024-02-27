#ifndef XML_PARSER_H
#define XML_PARSER_H

#include "source.h"
#include "document.h"
#include "markup.h"

#include <string_view>
#include <vector>
#include <string>
#include <sstream>

using std::stringstream;
using std::string_view;
using std::vector;
using std::string;

namespace scratchpad
{
	namespace xml
	{
		enum class parsing_states
		{
			ParsingStartTag   = 0,
			ParsingEndTag	  = 1,
			ParsingDeclAtts	  = 2,
			ParsingTypeAtts	  = 3,
			ParsingAtts		  = 4,
			ParsingType		  = 5,
			ParsingComment    = 6,
			ParsingContent	  = 7,
			ParsingUnknown	  = 8,
			ParsingStateCount = 9
		};

		class parser
		{

		public:

			explicit parser(xml::source* XMLSource);

			 parser();
			~parser();

		public:

			xml::document* Parse();
			void		   Close();

		private:
			/****
			***** XML MARKUP
			***** BUILDER METHODS
			****/
			void PushMarkup(markup_types MarkupType, 
							string MarkupText);

			void PopMarkup();

		private:
			/****
			***** XML UTILITY/  
			***** CONVENIENCE METHODS
			****/
			void Rewind(size_t Count);
			bool IsNL();
			bool IsWS();
			void TrimWS();

			/****
			***** STATE HANDLING 
			***** RELATED METHODS
			****/
			bool StateMatches(parsing_states XMLParsingState);
			void SwitchState(parsing_states NextState);

			/****
			***** MATCH BYTES 
			****/
			bool Match(const char* Bytes, size_t ByteCount);

		private:
			/****
			***** TOKEN MATCHING
			***** METHODS
			****/
			bool TryToParseNameStart();
			bool TryToParseNameToken(char Delim);
			bool TryToParseLiteral();
			bool TryToParseContent();

			bool TryToParseStartTag(string TagText);
			bool TryToParseEndTag(string TagText);

			bool TryToParseDeclStart();
			bool TryToParseDeclEnd();

			bool TryToParseTypeStart();
			bool TryToParseTypeEnd();

			bool TryToParseCommentStart();
			bool TryToParseCommentEnd();

		private:
			/****
			***** MAIN STATE PROCESSING &
			***** LEXING METHODS
			****/
			void ProcessState();
			void LexBuff();

		private:
			/****
			***** LEXER 
			***** META DATA
			****/
			size_t WSSkipped;
			size_t BytesRead;
			size_t BytesWritten;
			size_t LineCount;
			size_t Row;
			size_t Col;

			/****
			***** PARSING STATE 
			***** DATA
			****/
			xml::parsing_states ParsingState;
			xml::parsing_states LastParsingState;

			/****
			***** GENERIC STRING
			***** FOR WRITE BUFFERING
			****/
			string		 WriteBuff;
			string		 ExtraStringBuff;
			string		 ContentBuff;

			/****
			***** ERROR
			***** DATA
			****/
			bool		 Error;
			stringstream ErrorBuff;

			/****
			***** NAME
			***** TOKENS
			***** STACK
			****/
			vector<string>	NameTokens;
			vector<xml::markup*> Markup;

			/****
			***** XML SOURCE DATA
			****/
			xml::source& Source;
		};
	}
}

#endif
