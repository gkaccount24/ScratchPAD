#ifndef XML_PARSER_H
#define XML_PARSER_H

#include "diagnostics.h"
#include "source.h"
#include "document.h"
#include "markup.h"

#include <string_view>
#include <vector>
#include <string>
#include <sstream>

using std::streambuf;
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
			ParsingNameToken  = 7,
			ParsingContent	  = 8,
			ParsingLiteral    = 9,
			ParsingUnknown	  = 10,
			ParsingStateCount = 11
		};

		enum class buffer_idx
		{
			Markup = 0,
			NameToken = 1,
			Literal = 2,
			Content = 3,
			Count = 4
		};

		class parser
		{

		public:

			 parser();
			~parser();

		public:

			xml::document* Parse(xml::source* XMLSource);
			void		   Close();

		public:
			/****
			***** LOGGER/OUTPUT
			***** WRITING METHODS
			****/

			void OutputMessageBuffer(std::ostream& OutputConsole);
			void OutputMessageBuffer(std::ofstream& File);

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
			bool IsNL();
			bool IsWS();
			void TrimWS();

			/****
			***** STATE HANDLING 
			***** RELATED METHODS
			****/
			bool StateMatches(parsing_states XMLParsingState);
			bool LastStateMatches(parsing_states XMLParsingState);
			void SwitchState(parsing_states NextState);

			bool MarkupTypeMatches(xml::markup_types MarkupType);

		private:
			/****
			***** XML SOURCE DATA
			****/
			xml::source* Source;

			/****
			***** XML DOCUMENT DATA
			****/
			xml::document* NewDocument;

			/****
			***** ARRAY OF PARSED DOCUMENTS
			****/
			vector<xml::document*> ParsedDocuments;

			/****
			***** XML SOURCE BUFFER
			****/
			streambuf* Buffer;

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

			size_t MarkupAttributeIndex;
			size_t MarkupDepth;

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
			string Buffers[(int) buffer_idx::Count];

			/****
			***** ERROR & 
			***** PARSER MESSAGE BUFFER 
			***** FOR WRITING/LOGGING
			***** DATA
			****/
			bool		 Error;

			/****
			***** NAME
			***** TOKENS & MARKUP
			***** STACKS
			****/
			vector<string>		 NameTokens;
			vector<xml::markup*> Markup;

			/****
			***** Expected Char
			****/

			char			  ExpectedByte;
			string			  ExpectedNameTok;
			string			  ExpectedMarkup;
			xml::markup_types ExpectedType;

		public:
			/****
			***** XML PARSER DIAGNOSTICS
			****/
			xml::diagnostics* Diagnostics;

		};
	}
}

#endif
