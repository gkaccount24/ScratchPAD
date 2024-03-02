#include "parser_diagnostics.h"

#define EMPTY_BUFFER_STRING ""

namespace scratchpad
{
	namespace xml
	{
		/****
		***** DIAGNOSTIC CONSTRUCTOR
		****/
		parser_diagnostics::parser_diagnostics(stringstream& ParserMessageBuffer):
			MessageBuffer(ParserMessageBuffer),
			WriteBuffer(EMPTY_BUFFER_STRING),
			ErrorCode(xml::error_codes::Unknown),
			Error(false)
		{ }

		/****
		***** DIAGNOSTIC DESTRUCTOR 
		****/
		parser_diagnostics::~parser_diagnostics() 
		{
			WriteInfoLog("destroying parser_diagnostics");

			Error = false;
		}

		inline void parser_diagnostics::WriteErrorLog(string Message) 
		{
			MessageBuffer << "[ERROR] " << Message 
						  << "\n";
		}

		inline void parser_diagnostics::WriteWarningLog(string Message) 
		{
			MessageBuffer << "[WARNING] " << Message
					  	  << "\n";
		}

		inline void parser_diagnostics::WriteInfoLog(string Message) 
		{
			MessageBuffer << "[INFO] " << Message
					  	  << "\n";
		}

		inline bool parser_diagnostics::SetErrorState(xml::error_codes XMLErrorCode)
		{
			if(!Error)
			{
				WriteInfoLog("setting error state...");

				Error = true;
				ErrorCode = XMLErrorCode;

				return Error;
			}

			return !Error;
		}

		inline void parser_diagnostics::SetErrorMalformedNameToken(string NameToken)
		{
			if(SetErrorState(xml::error_codes::MalformedNameToken))
			{
				MessageBuffer << "received malformed name token: "
							  << NameToken << "\n";

				WriteBuffer.clear();
			}
		}

		inline void parser_diagnostics::SetErrorMalformedStartTag()
		{
			if(SetErrorState(xml::error_codes::MalformedStartTag))
			{
				// ReadBuff(BytesRead);

				MessageBuffer << "received malformed start tag: "
							  << WriteBuffer << "\n";

				WriteBuffer.clear();
			}
		}

		inline void parser_diagnostics::SetErrorMissingEndTag(string ExpectedText, string ReceivedText)
		{
			if(SetErrorState(xml::error_codes::MissingEndTag))
			{
				MessageBuffer << "error missing end tag\n"
					<< "received token: " << ReceivedText
					<< "expected token: " << ExpectedText
					<< "\n";

				WriteBuffer.clear();
			}
		}

		inline void parser_diagnostics::SetErrorMissingXMLVersionAttribute()
		{
			if(SetErrorState(xml::error_codes::MissingXMLVersionAttrib))
			{
				MessageBuffer << "error missing xml version attribute"
							  << "\n";
			}
		}

		inline void parser_diagnostics::SetErrorOutOfOrderDeclAttribute(int ReceivedOrder, int ExpectedOrder)
		{
			if(SetErrorState(xml::error_codes::OutOfOrderDeclAttribute))
			{
				MessageBuffer << "out of order decl attribute"
							  << "\n";
			}
		}

		inline void parser_diagnostics::SetErrorFileNotOpen(const char* XMLSourceDiskPath)
		{
			if(SetErrorState(xml::error_codes::FileNotFound))
			{
				MessageBuffer << "failed to open file "
						      << XMLSourceDiskPath << "\n";
			}
		}

		inline void parser_diagnostics::SetErrorFailedToReadFile(const char* XMLSourceDiskPath)
		{
			if(SetErrorState(xml::error_codes::FailedToReadFile))
			{
				MessageBuffer << "failed to read file "
							  << XMLSourceDiskPath << "\n";
			}
		}

		inline void parser_diagnostics::SetErrorMissingQuotes()
		{
			if(SetErrorState(xml::error_codes::MissingQuotes))
			{
				MessageBuffer << "[ERROR] literal value missing quotes"
							  << "\n";
			}
		}

		inline void parser_diagnostics::SetErrorUnclosedTag()
		{
			if(SetErrorState(xml::error_codes::UnclosedErrorTag))
			{
				MessageBuffer << "[ERROR] unclosed xml tag"
							  << "\n";
			}
		}

		inline void parser_diagnostics::SetErrorMissingAttVal()
		{
			if(SetErrorState(xml::error_codes::MissingAttVal))
			{
				MessageBuffer << "[ERROR] missing attribute value"
							  << "\n";
			}
		}

		inline void parser_diagnostics::SetErrorIllegalNameStart(char IllegalCharacter)
		{
			if(SetErrorState(xml::error_codes::IllegalNameStart))
			{
				MessageBuffer << "[ERROR] illegal name start char\n"
							  << "        received " << IllegalCharacter << "\n";
			}
		}

		inline void parser_diagnostics::SetErrorIllegalLiteralVal(char IllegalCharacter)
		{
			if(SetErrorState(xml::error_codes::IllegalLiteralVal))
			{
				MessageBuffer << "[ERROR] illegal literal value\n";
				MessageBuffer << "        received " << IllegalCharacter << "\n";
			}
		}

		inline void parser_diagnostics::SetErrorMissingWS()
		{
			if(SetErrorState(xml::error_codes::MissingWS))
			{
				MessageBuffer << "[ERROR] missing required "
					<< "whitespace\n";
			}
		}

		inline void parser_diagnostics::SetErrorMalformedTypeTag(string NameToken)
		{
			if(SetErrorState(xml::error_codes::MalformedTypeTag))
			{
				MessageBuffer << "[ERROR] malformed xml decl tag\n"
							  << "        received " << NameToken << "\n";
			}
		}

		inline void parser_diagnostics::SetErrorMalformedDeclTag(string NameToken)
		{
			if(SetErrorState(xml::error_codes::MalformedDeclTag))
			{
				MessageBuffer << "[ERROR] malformed xml decl tag\n"
							  << "        received " << NameToken << "\n";
			}
		}

		inline void parser_diagnostics::SetErrorMalformedCommentTag()
		{
			if(SetErrorState(xml::error_codes::MalformedCommentTag))
			{
				MessageBuffer << "[ERROR] malformed comment tag\n";
			}
		}

		inline void parser_diagnostics::SetErrorEncounteredInvalidStartTag(string Tag)
		{
			if(SetErrorState(xml::error_codes::InvalidStartTag))
			{
				MessageBuffer << "[ERROR] invalid start tag encountered\n"
							  << "        START TAG" << Tag << "\n";
			}
		}

		inline void parser_diagnostics::SetErrorMissingAttribVal()
		{
			if(SetErrorState(xml::error_codes::MissingAttribVal))
			{
				MessageBuffer << "[ERROR] missing attrib val\n";
			}
		}
	}
}