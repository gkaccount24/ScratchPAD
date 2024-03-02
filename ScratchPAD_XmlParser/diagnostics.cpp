#include "diagnostics.h"

#define EMPTY_BUFFER_STRING ""

namespace scratchpad
{
	namespace xml
	{
		/****
		***** DIAGNOSTIC CONSTRUCTOR
		****/
		diagnostics::diagnostics():
			MessageBuffer(EMPTY_BUFFER_STRING),
			WriteBuffer(EMPTY_BUFFER_STRING),
			ExitCode(xml::error_codes::Success),
			Error(false)
		{ }

		/****
		***** DIAGNOSTIC DESTRUCTOR 
		****/
		diagnostics::~diagnostics() 
		{
			WriteInfoLog("destroying diagnostics");

			Error = false;
		}

		void diagnostics::WriteErrorLog(string Message) 
		{
			MessageBuffer << "[ERROR] " << Message 
						  << "\n";
		}

		void diagnostics::WriteWarningLog(string Message) 
		{
			MessageBuffer << "[WARNING] " << Message
					  	  << "\n";
		}

		void diagnostics::WriteInfoLog(string Message) 
		{
			MessageBuffer << "[INFO] " << Message
					  	  << "\n";
		}

		bool diagnostics::SetErrorState(xml::error_codes XMLErrorCode)
		{
			if(!Error)
			{
				WriteInfoLog("setting error state...");

				Error	 = true;
				ExitCode = XMLErrorCode;

				return Error;
			}

			return !Error;
		}

		void diagnostics::SetErrorMalformedNameToken(string NameToken)
		{
			if(SetErrorState(xml::error_codes::MalformedNameToken))
			{
				MessageBuffer << "received malformed name token: "
							  << NameToken << "\n";

				WriteBuffer.clear();
			}
		}

		void diagnostics::SetErrorMalformedStartTag()
		{
			if(SetErrorState(xml::error_codes::MalformedStartTag))
			{
				// ReadBuff(BytesRead);

				MessageBuffer << "received malformed start tag: "
							  << WriteBuffer << "\n";

				WriteBuffer.clear();
			}
		}

		void diagnostics::SetErrorMissingEndTag(string ExpectedText, string ReceivedText)
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

		void diagnostics::SetErrorMissingXMLVersionAttribute()
		{
			if(SetErrorState(xml::error_codes::MissingXMLVersionAttrib))
			{
				MessageBuffer << "error missing xml version attribute"
							  << "\n";
			}
		}

		void diagnostics::SetErrorOutOfOrderDeclAttribute(int ReceivedOrder, int ExpectedOrder)
		{
			if(SetErrorState(xml::error_codes::OutOfOrderDeclAttribute))
			{
				MessageBuffer << "out of order decl attribute"
							  << "\n";
			}
		}

		void diagnostics::SetErrorFileNotOpen(const char* XMLSourceDiskPath)
		{
			if(SetErrorState(xml::error_codes::FileNotFound))
			{
				MessageBuffer << "failed to open file "
						      << XMLSourceDiskPath << "\n";
			}
		}

		void diagnostics::SetErrorFailedToReadFile(const char* XMLSourceDiskPath)
		{
			if(SetErrorState(xml::error_codes::FailedToReadFile))
			{
				MessageBuffer << "failed to read file "
							  << XMLSourceDiskPath << "\n";
			}
		}

		void diagnostics::SetErrorMissingQuotes()
		{
			if(SetErrorState(xml::error_codes::MissingQuotes))
			{
				MessageBuffer << "[ERROR] literal value missing quotes"
							  << "\n";
			}
		}

		void diagnostics::SetErrorUnclosedTag()
		{
			if(SetErrorState(xml::error_codes::UnclosedErrorTag))
			{
				MessageBuffer << "[ERROR] unclosed xml tag"
							  << "\n";
			}
		}

		void diagnostics::SetErrorMissingAttVal()
		{
			if(SetErrorState(xml::error_codes::MissingAttVal))
			{
				MessageBuffer << "[ERROR] missing attribute value"
							  << "\n";
			}
		}

		void diagnostics::SetErrorIllegalNameStart(char IllegalCharacter)
		{
			if(SetErrorState(xml::error_codes::IllegalNameStart))
			{
				MessageBuffer << "[ERROR] illegal name start char\n"
							  << "        received " << IllegalCharacter << "\n";
			}
		}

		void diagnostics::SetErrorIllegalLiteralVal(char IllegalCharacter)
		{
			if(SetErrorState(xml::error_codes::IllegalLiteralVal))
			{
				MessageBuffer << "[ERROR] illegal literal value\n";
				MessageBuffer << "        received " << IllegalCharacter << "\n";
			}
		}

		void diagnostics::SetErrorMissingWS()
		{
			if(SetErrorState(xml::error_codes::MissingWS))
			{
				MessageBuffer << "[ERROR] missing required "
					<< "whitespace\n";
			}
		}

		void diagnostics::SetErrorMalformedTypeTag(string NameToken)
		{
			if(SetErrorState(xml::error_codes::MalformedTypeTag))
			{
				MessageBuffer << "[ERROR] malformed xml decl tag\n"
							  << "        received " << NameToken << "\n";
			}
		}

		void diagnostics::SetErrorMalformedDeclTag(string NameToken)
		{
			if(SetErrorState(xml::error_codes::MalformedDeclTag))
			{
				MessageBuffer << "[ERROR] malformed xml decl tag\n"
							  << "        received " << NameToken << "\n";
			}
		}

		void diagnostics::SetErrorMalformedCommentTag()
		{
			if(SetErrorState(xml::error_codes::MalformedCommentTag))
			{
				MessageBuffer << "[ERROR] malformed comment tag\n";
			}
		}

		void diagnostics::SetErrorEncounteredInvalidStartTag(string Tag)
		{
			if(SetErrorState(xml::error_codes::InvalidStartTag))
			{
				MessageBuffer << "[ERROR] invalid start tag encountered\n"
							  << "        START TAG" << Tag << "\n";
			}
		}

		void diagnostics::SetErrorMissingAttribVal()
		{
			if(SetErrorState(xml::error_codes::MissingAttribVal))
			{
				MessageBuffer << "[ERROR] missing attrib val\n";
			}
		}
	}
}