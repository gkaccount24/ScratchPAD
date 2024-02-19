#include "./include/xml_source.h"

/****
***** TYPE CASTING
***** HELPER MACROS
****/

#define StaticCast(Type, Val) static_cast<Type>((Val))
#define HardCast(Type, Val) reinterpret_cast<Type>((Val))
#define DynamicCast(Type, Val) dynamic_cast<Type>((Val))

namespace scratchpad
{
	/****
	***** MAIN CONSTRUCTOR
	***** FOR XML SOURCE
	****/
	xml_source::xml_source(string XMLSourceBuff):
		SourceBuff(move(XMLSourceBuff)),
		SourceBuffSize(XMLSourceBuff.size()),
		WSSkipped(0),
		BytesRead(0),
		BytesWritten(0),
		LineCount(1),
		Row(0), Col(0),
		Error(false), 
		ErrorBuff("")
	{
		
	}

	xml_source::xml_source(fstream&& XMLSourceFile, 
						   string XMLSourceDiskPath):
		SourceBuff(""),
		WSSkipped(0),
		BytesRead(0),
		BytesWritten(0),
		LineCount(1),
		Row(0), Col(0),
		Error(false), 
		ErrorBuff("")

	{
		if(!ReadSourceFile(move(XMLSourceFile), XMLSourceDiskPath.c_str()))
		{
			// failed to open source file
			// log failure to open xml source
			SetErrorFailedToReadFile(XMLSourceDiskPath.c_str());
		}
		else
		{
			// SourceBuff.rdbuf(XMLSourceFile.rdbuf())

		}
	}

	/****
	***** MOVE CONSTRUCTOR FILE 
	***** FOR XML SOURCE
	****/
	xml_source::xml_source(xml_source&& RSource) noexcept:
		SourceBuff(move(RSource.SourceBuff)),
		SourceBuffSize(RSource.SourceBuffSize),
		WSSkipped(RSource.WSSkipped),
		BytesRead(RSource.BytesRead),
		BytesWritten(RSource.BytesWritten),
		LineCount(RSource.LineCount),
		Row(RSource.Row), Col(RSource.Col),
		Error(RSource.Error), 
		ErrorBuff(move(RSource.ErrorBuff))
	{
		
	}

	xml_source::~xml_source()
	{
		Close();
	}

	/****
	***** CLEANUP METHOD/FILE 
	****/
	void xml_source::Close()
	{
		if(SourceFile.is_open())
		{
			SourceFile.clear();
			SourceFile.close();
		}

		NameTokens.clear();
		Markup.clear();

		WSSkipped = 0;
		BytesRead = 0;
		BytesWritten = 0;
		LineCount = 0;
		Row = 0;
		Col = 0;

		SourceBuff.clear();
		SourceBuffSize = 0;
	}

	void xml_source::SetSourceBuff(string XMLSourceBuff)
	{
		SourceBuffSize = XMLSourceBuff.size();
		SourceBuff.str(move(XMLSourceBuff));
	}

	bool xml_source::ReadSourceFile(fstream&& SourceFile, string XMLSourceDiskPath)
	{
		if(!SourceFile.is_open())
		{
			// failed to open source file
			// log failure to open xml source
			SetErrorFileNotOpen(XMLSourceDiskPath.c_str());
			return false;
		}

		SourceFile.seekg(0, std::ios_base::end);
		SourceBuffSize = SourceFile.tellg();
		SourceFile.seekg(0, std::ios_base::beg);

		string TempBuff;

		TempBuff.resize(SourceBuffSize);

		SourceFile.read(TempBuff.data(), 
						TempBuff.size());

		SourceFile.close();

		SourceBuff.str(move(TempBuff));

		return true;
	}

	void xml_source::PopMarkup()
	{
		if(!Markup.empty())
		{
			Markup.pop_back();
		}
	}

	void xml_source::PushMarkup(string NameToken)
	{
		xml_markup* MarkupNode = nullptr;

		if(MarkupNode = new xml_markup(move(NameToken)))
		{
			if(!Markup.empty())
			{
				Markup.back()->Children.push_back(MarkupNode);
			}

			Markup.push_back(MarkupNode);
		}
	}

	/****
	***** MAIN SOURCE PARSING METHOD
	***** PARSES XML FILES OF COURSE
	****/
	xml_document* xml_source::Parse()
	{
		xml_document* Doc = nullptr;

		while(Buffer()->in_avail() > 0 && !Error)
		{
			TrimWS();

			char DebugChar = Buffer()->sgetc();

			switch(Buffer()->sgetc())
			{
				/****
				***** PARSE XML STARTING ELEMENT
				****/
				case '<':
				{
					if(TryToParseDeclStart())
					{
						const string StartTag = "<?xml";
						PushMarkup(StartTag);

						break;
					}
					else if(TryToParseTypeStart())
					{
						const string StartTag = "<!DOCTYPE";
						PushMarkup(StartTag);

						break;
					}
					else if(TryToParseCommentStart())
					{
						const string StartTag = "<!--";
						PushMarkup(StartTag);

						break;
					}
					else
					{
						// advance input buffer
						// fewer bytes to read;
						Buffer()->sbumpc();

						if(!TryToParseNameToken('>'))
						{
							// OutputLastError();
							break;
						}
					}

					break;
				}

				/****
				***** PARSE XML ENDING ELEMENT
				****/
				case '>':
				{

					break;
				}

				/****
				***** PARSE LITERALS 
				****/
				case '\"':
				{
					if(!TryToParseLiteral())
					{

					}

					break;
				}
			}
		}

		return Doc;
	}

	/****
	***** PRIVATE PARSING METHODS
	****/
	bool xml_source::SetErrorState()
	{
		if(!Error)
		{
			Error = true;
			return Error;
		}

		return !Error;
	}

	void xml_source::SetErrorFileNotOpen(const char* XMLSourceDiskPath)
	{
		if(SetErrorState())
		{
			ErrorBuff << "failed to open file " 
				      << XMLSourceDiskPath << endl;
		}
	}

	void xml_source::SetErrorFailedToReadFile(const char* XMLSourceDiskPath)
	{
		if(SetErrorState())
		{
			ErrorBuff << "failed to read file " 
				      << XMLSourceDiskPath << endl;
		}
	}

	void xml_source::SetErrorUnclosedTag()
	{
		if(SetErrorState())
		{
			ErrorBuff << "[ERROR] unclosed xml tag"
					  << endl;
		}
	}

	void xml_source::SetErrorMissingAttVal()
	{
		if(SetErrorState())
		{
			ErrorBuff << "[ERROR] missing attribute value"
					  << endl;
		}
	}

	void xml_source::SetErrorIllegalNameStart()
	{
		if(SetErrorState())
		{
			ErrorBuff << "[ERROR] illegal name start char\n"
					  << "        received " << Buffer()->sgetc() << endl;
		}
	}

	void xml_source::SetErrorIllegalLiteralVal()
	{
		if(SetErrorState())
		{ 
			ErrorBuff << "[ERROR] illegal literal value\n";
			ErrorBuff << "        received " << Buffer()->sgetc() << endl;
		}
	}

	void xml_source::SetErrorMalformedDeclTag()
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

	void xml_source::Rewind(size_t Count)
	{
		while(Count > 0)
		{
			// add bytes back
			// to the input buffer
			Buffer()->sungetc();

			Count--;
		}
	}

	string xml_source::GetLastError()
	{
		return ErrorBuff.str();
	}

	void xml_source::OutputLastError()
	{
		string ErrorMessage = GetLastError();

		if(!ErrorMessage.empty())
		{
			cout << ErrorMessage 
				 << endl;
		}
	}

	streambuf* xml_source::Buffer()
	{
		return SourceBuff.rdbuf();
	}

    bool xml_source::IsNL()
    {
        return (Buffer()->sgetc() == '\r' ||
                Buffer()->sgetc() == '\n');
    }

    bool xml_source::IsWS()
    {
        return (Buffer()->sgetc() == ' '  ||
		        Buffer()->sgetc() == '\t' ||
                Buffer()->sgetc() == '\r' ||
                Buffer()->sgetc() == '\n');
    }

    void xml_source::TrimWS()
    {
        WSSkipped = 0;

        while(Buffer()->in_avail() > 0 && IsWS() && !Error)
        {
            if(IsNL())
            {
                LineCount++;
                Row++;
            }

            WSSkipped++;

            // advance input buffer
            // less bytes to read after a bump
            Buffer()->sbumpc();
        }
    }

	bool xml_source::Match(const char* Bytes, size_t ByteCount)
	{
		if(ByteCount > Buffer()->in_avail())
		{
			return false;
		}

		BytesRead = 0;

		bool Matched = true;

		for(size_t Idx = 0; Idx < ByteCount; Idx++)
		{
			char CDebug = Buffer()->sgetc();

			if(Buffer()->sgetc() != Bytes[Idx])
			{
				Matched = false;
				break;
			}

			// advance input 
			// buffer
			Buffer()->sbumpc();
			BytesRead++;
		}

		return Matched && !Error;
	}

	bool xml_source::TryToParseDeclStart()
	{		
		const string StartTag = "<?xml";

		if(Match(StartTag.c_str(),
				 StartTag.size()))
		{
			if(!IsWS())
			{
				// grab last byte
				// read for error 
				// reporting
				BytesRead++;

				SetErrorMalformedDeclTag();
			}

			return !Error;
		}

		Rewind(StartTag.size());

		return false;
	}

	bool xml_source::TryToParseTypeStart()
	{
		const string StartTag = "<!DOCTYPE";

		if(Match(StartTag.c_str(),
				 StartTag.size()))
		{
			return !Error;
		}

		Rewind(StartTag.size());

		return false;
	}

	bool xml_source::TryToParseCommentStart()
	{
		const string StartTag = "<!--";

		if(Match(StartTag.c_str(),
				 StartTag.size()))
		{
			return !Error;
		}

		Rewind(StartTag.size());
		
		return false;
	}

	bool xml_source::TryToParseNameStart()
	{
		/*** [4]		NameStartChar	   :: = ":" | [A - Z] | "_" | [a - z] | [#xC0 - #xD6] | 
		****                                          [#xD8 - #xF6] | [#xF8 - #x2FF] | [#x370 - #x37D] | 
		****                                          [#x37F - #x1FFF] | [#x200C - #x200D] | [#x2070 - #x218F] | 
		****                                          [#x2C00 - #x2FEF] | [#x3001 - #xD7FF] | [#xF900 - #xFDCF] | 
		****                                          [#xFDF0 - #xFFFD] | [#x10000 - #xEFFFF]*/
		const uint8_t UpperCaseLower = StaticCast(int, 'A');
		const uint8_t UpperCaseHigher = StaticCast(int, 'Z');

		const uint8_t LowerCaseLower = StaticCast(int, 'a');
		const uint8_t LowerCaseHigher = StaticCast(int, 'z');

		bool IsLower = (StaticCast(uint8_t, Buffer()->sgetc()) >= LowerCaseLower &&
					    StaticCast(uint8_t, Buffer()->sgetc()) <= LowerCaseHigher);
		bool IsUpper = (StaticCast(uint8_t, Buffer()->sgetc()) >= UpperCaseLower &&
						StaticCast(uint8_t, Buffer()->sgetc()) <= UpperCaseHigher);

		bool IsUnderscore = Buffer()->sgetc() == '_';

		return(IsLower || IsUpper || IsUnderscore);
	}

	bool xml_source::TryToParseNameToken(char Delim)
	{
		/*** [4]		NameStartChar	   :: = ":" | [A - Z] | "_" | [a - z] | [#xC0 - #xD6] | 
		****                                          [#xD8 - #xF6] | [#xF8 - #x2FF] | [#x370 - #x37D] | 
		****                                          [#x37F - #x1FFF] | [#x200C - #x200D] | [#x2070 - #x218F] | 
		****                                          [#x2C00 - #x2FEF] | [#x3001 - #xD7FF] | [#xF900 - #xFDCF] | 
		****                                          [#xFDF0 - #xFFFD] | [#x10000 - #xEFFFF]
		**** [4a]   	NameChar	   :: = NameStartChar | "-" | "." | [0 - 9] | #xB7 | [#x0300 - #x036F] | [#x203F - #x2040]
		**** [5]   	Name	   :: = NameStartChar(NameChar) *
		**** [6]   	Names	   :: = Name(#x20 Name) *
		**** [7]   	Nmtoken	   :: = (NameChar)+
		**** [8]   	Nmtokens	   :: = Nmtoken(#x20 Nmtoken) 
		***/

		if(!TryToParseNameStart())
		{
			SetErrorIllegalNameStart();
			return !Error;
		}

		// reset byte counter
		// to track name token size
		BytesRead = 0;

		// we need to know what delim
		// we parsed to
		bool ParsedToDelim = false;
		bool ParsedToWS = false;

		while(Buffer()->in_avail() > 0)
		{
			if(Buffer()->sgetc() == Delim)
			{
				ParsedToDelim = true;
				break;
			}
			else if(BytesRead > 0 && IsWS())
			{
				ParsedToWS = true;
				break;
			}
			else
			{
				BytesRead++;

				// advance buffer
				// fewer bytes to read
				Buffer()->sbumpc();
			}
		}

		if(ParsedToDelim || ParsedToWS)
		{
			for(size_t I = 0; I < BytesRead; I++)
			{
				Buffer()->sungetc();
			}

			// make temp buff
			string TempBuff;
			TempBuff.resize(BytesRead);

			// extract bytes from 
			// buffer into temp buffer
			Buffer()->sgetn(TempBuff.data(), TempBuff.size());

			// save token text 
			// on token stack
			NameTokens.push_back(move(TempBuff));
		}

		return ParsedToDelim && !Error;
	}

	bool xml_source::TryToParseLiteral()
	{
		const char IllegalCharacters[] 
		{
			'<', '&'
		};

		while(Buffer()->in_avail() > 0  && 
			  Buffer()->sgetc() != '\"' && 
			  !Error)
		{
			if(Buffer()->sgetc() == IllegalCharacters[0] && 
			   Buffer()->sgetc() == IllegalCharacters[1])
			{
				SetErrorIllegalLiteralVal();
				break;
			}

			Buffer()->sbumpc();
		}

		return !Error;
	}
}
