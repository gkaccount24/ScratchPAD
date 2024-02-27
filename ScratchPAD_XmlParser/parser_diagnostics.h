#ifndef PARSER_DIAGNOSTICS_H
#define PARSER_DIAGNOSTICS_H

#include <string>
#include <sstream>

using std::string;
using std::stringstream;

namespace scratchpad
{
	namespace xml
	{
		class parser_diagnostics
		{

		public:

			 parser_diagnostics();
			~parser_diagnostics();

		public:

			bool SetErrorState();
			j
			void SetErrorIllegalNameStart();
			void SetErrorIllegalLiteralVal();

			void SetErrorMalformedDeclTag();
			void SetErrorMalformedNameToken();
			void SetErrorMalformedStartTag();
			void SetErrorMalformedTypeTag();
			void SetErrorMalformedCommentTag();

			void SetErrorUnclosedTag();

			void SetErrorMissingAttVal();
			void SetErrorMissingEndTag(string ExpectedText);
			void SetErrorMissingQuotes();
			void SetErrorMissingAttribVal();
			void SetErrorMissingWS();

			void SetErrorEncounteredInvalidStartTag();
			void SetErrorMissingXMLVersionAttribute();

			void SetErrorOutOfOrderDeclAttribute(int ReceivedOrder, int ExpectedOrder);
			void SetErrorFileNotOpen(const char* XMLSourceDiskPath);
			void SetErrorFailedToReadFile(const char* XMLSourceDiskPath);

		};
	}
}


#endif

