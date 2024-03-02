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
		enum error_codes
		{
			Success			 = 0,
			IllegalNameStart = -100,
			IllegalLiteralVal = -101,
			MalformedDeclTag = -102,
			MalformedNameToken = -103,
			MalformedStartTag = -104,
			MalformedTypeTag = -105,
			MalformedCommentTag = -106,
			UnclosedErrorTag = -107,
			MissingAttVal = -108,
			MissingEndTag = -109,
			MissingQuotes = -110,
			MissingAttribVal = -111,
			MissingWS = -112,
			InvalidStartTag = -113,
			MissingXMLVersionAttrib = -114,
			OutOfOrderDeclAttribute = -115,
			FileNotFound = -116,
			FailedToReadFile = -117,
			Unknown
		};

		class diagnostics
		{

		private:

			 diagnostics();
			~diagnostics();

		public:

			static diagnostics* ParserDiagnostics()
			{
				static diagnostics Diagnostics;

				return &Diagnostics;
			}

		public:

			/**
			*** HELPER LOGGING METHODS
			**/
			void WriteErrorLog(string Message);
			void WriteWarningLog(string Message);
			void WriteInfoLog(string Message);

		public:

			bool SetErrorState(xml::error_codes XMLErrorCode);
			void SetErrorIllegalNameStart(char IllegalChar);
			void SetErrorIllegalLiteralVal(char IllegaChar);

			void SetErrorMalformedDeclTag(string Tag);
			void SetErrorMalformedNameToken(string NameToken);
			void SetErrorMalformedStartTag();
			void SetErrorMalformedTypeTag(string Tag);
			void SetErrorMalformedCommentTag();

			void SetErrorUnclosedTag();

			void SetErrorMissingAttVal();
			void SetErrorMissingEndTag(string ExpectedText, string ReceivedText);
			void SetErrorMissingQuotes();
			void SetErrorMissingAttribVal();
			void SetErrorMissingWS();

			void SetErrorEncounteredInvalidStartTag(string Tag);
			void SetErrorMissingXMLVersionAttribute();

			void SetErrorOutOfOrderDeclAttribute(int ReceivedOrder, int ExpectedOrder);
			void SetErrorFileNotOpen(const char* XMLSourceDiskPath);
			void SetErrorFailedToReadFile(const char* XMLSourceDiskPath);

		private:

			stringstream MessageBuffer;
			string		 WriteBuffer;

		public:
			error_codes   ExitCode;
			bool		  Error;

		};
	}
}

#endif

