#include "parser_diagnostics.h"

namespace scratchpad
{
	namespace xml
	{
		parser_diagnostics::parser_diagnostics() { }
		parser_diagnostics::~parser_diagnostics() { }

		inline bool parser_diagnostics::SetErrorState()
		{
			if(!Error)
			{
				Error = true;
				return Error;
			}

			return !Error;
		}

		inline void parser_diagnostics::SetErrorMalformedNameToken()
		{
			if(SetErrorState())
			{
				ReadBuff(BytesRead);

				ErrorBuff << "received malformed name token: "
					<< ExtraStringBuff << "\n";

				ExtraStringBuff.clear();
			}
		}

		inline void parser_diagnostics::SetErrorMalformedStartTag()
		{
			if(SetErrorState())
			{
				ReadBuff(BytesRead);

				ErrorBuff << "received malformed start tag: "
					<< ExtraStringBuff << "\n";

				ExtraStringBuff.clear();
			}
		}

		inline void parser_diagnostics::SetErrorMissingEndTag(string ExpectedText)
		{
			if(SetErrorState())
			{
				ReadBuff(BytesRead);

				ErrorBuff << "error missing end tag\n"
					<< "received token: " << ExtraStringBuff
					<< "expected token: " << ExpectedText
					<< endl;

				ExtraStringBuff.clear();
			}
		}

		inline void parser_diagnostics::SetErrorMissingXMLVersionAttribute()
		{
			if(SetErrorState())
			{
				ErrorBuff << "error missing xml version attribute"
					<< endl;
			}
		}

		inline void parser_diagnostics::SetErrorOutOfOrderDeclAttribute(int ReceivedOrder, int ExpectedOrder)
		{
			if(SetErrorState())
			{
				ErrorBuff << "out of order decl attribute"
					<< endl;
			}
		}

		inline void parser_diagnostics::SetErrorFileNotOpen(const char* XMLSourceDiskPath)
		{
			if(SetErrorState())
			{
				ErrorBuff << "failed to open file "
					<< XMLSourceDiskPath << endl;
			}
		}

		inline void parser_diagnostics::SetErrorFailedToReadFile(const char* XMLSourceDiskPath)
		{
			if(SetErrorState())
			{
				ErrorBuff << "failed to read file "
					<< XMLSourceDiskPath << endl;
			}
		}

		inline void parser_diagnostics::SetErrorMissingQuotes()
		{
			if(SetErrorState())
			{
				ErrorBuff << "[ERROR] literal value missing quotes"
					<< endl;
			}
		}

		inline void parser_diagnostics::SetErrorUnclosedTag()
		{
			if(SetErrorState())
			{
				ErrorBuff << "[ERROR] unclosed xml tag"
					<< endl;
			}
		}

		inline void parser_diagnostics::SetErrorMissingAttVal()
		{
			if(SetErrorState())
			{
				ErrorBuff << "[ERROR] missing attribute value"
					<< endl;
			}
		}

		inline void parser_diagnostics::SetErrorIllegalNameStart()
		{
			if(SetErrorState())
			{
				ErrorBuff << "[ERROR] illegal name start char\n"
					<< "        received " << Buffer()->sgetc() << endl;
			}
		}

		inline void parser_diagnostics::SetErrorIllegalLiteralVal()
		{
			if(SetErrorState())
			{
				ErrorBuff << "[ERROR] illegal literal value\n";
				ErrorBuff << "        received " << Buffer()->sgetc() << endl;
			}
		}

		inline void parser_diagnostics::SetErrorMissingWS()
		{
			if(SetErrorState())
			{
				ErrorBuff << "[ERROR] missing required "
					<< "whitespace\n";
			}
		}

		inline void parser_diagnostics::SetErrorMalformedTypeTag()
		{
			if(SetErrorState())
			{
				string MalformedNameToken;

				MalformedNameToken.resize(BytesRead);

				Buffer()->sgetn(MalformedNameToken.data(),
								MalformedNameToken.size());

				ErrorBuff << "[ERROR] malformed xml decl tag\n"
					<< "        received " << MalformedNameToken
					<< endl;
			}
		}

		inline void parser_diagnostics::SetErrorMalformedDeclTag()
		{
			if(SetErrorState())
			{
				string MalformedNameToken;

				MalformedNameToken.resize(BytesRead);

				Buffer()->sgetn(MalformedNameToken.data(),
								MalformedNameToken.size());

				ErrorBuff << "[ERROR] malformed xml decl tag\n"
					<< "        received " << MalformedNameToken
					<< endl;
			}
		}

		inline void parser_diagnostics::SetErrorMalformedCommentTag()
		{
			if(SetErrorState())
			{
				ErrorBuff << "[ERROR] malformed comment tag\n"
					<< endl;
			}
		}

		inline void parser_diagnostics::SetErrorEncounteredInvalidStartTag()
		{
			if(SetErrorState())
			{
				ReadBuff(BytesRead);

				ErrorBuff << "[ERROR] invalid start tag encountered\n"
					<< "        START TAG" << ExtraStringBuff
					<< endl;
			}
		}

		inline void parser_diagnostics::SetErrorMissingAttribVal()
		{
			if(SetErrorState())
			{
				ErrorBuff << "[ERROR] missing attrib val\n"
					<< endl;
			}
		}
	}
}