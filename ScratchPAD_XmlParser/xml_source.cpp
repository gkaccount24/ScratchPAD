#include "./include/xml_source.h"

/****
***** C++ INCLUDES
****/

#include <string_view>

using std::string_view;

/****
***** TYPE CASTING
***** HELPER MACROS
****/

#define StaticCast(Type, Val) static_cast<Type>((Val))
#define HardCast(Type, Val) reinterpret_cast<Type>((Val))
#define DynamicCast(Type, Val) dynamic_cast<Type>((Val))

#define XMLDeclVersionAttributeIndex 0
#define XMLDeclEncodingAttributeIndex 1
#define XMLDeclStandaloneAttributeIndex 2

namespace scratchpad
{
	static const string_view XMLDeclStartTag = "<?xml";
	static const string_view XMLDeclEndTag	 = "?>";
	static const string_view XMLDocTypeStartTag = "<!DOCTYPE";
	static const string_view XMLDocTypeEndTag = ">";
	static const string_view CommentStartTag = "<!--";
	static const string_view CommentEndTag   = "-->";

	static const string_view XMLDeclVersionAttribute = "version";
	static const string_view XMLDeclEncodingAttribute = "encoding";
	static const string_view XMLDeclStandaloneAttribute = "standalone";

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
		ParsingState(xml_parsing_states::ParsingStartTag),
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
		ParsingState(xml_parsing_states::ParsingStartTag),
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
		ParsingState(RSource.ParsingState),
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
	inline void xml_source::Close()
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

	inline void xml_source::SetSourceBuff(string XMLSourceBuff)
	{
		SourceBuffSize = XMLSourceBuff.size();
		SourceBuff.str(move(XMLSourceBuff));
	}

	inline bool xml_source::ReadSourceFile(fstream&& SourceFile, string XMLSourceDiskPath)
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

	inline void xml_source::PopMarkup()
	{
		if(!Markup.empty())
		{
			Markup.pop_back();
		}
	}

	inline void xml_source::PushMarkup(xml_markup_types MarkupType, 
									   string MarkupText)
	{
		xml_markup* MarkupNode = nullptr;

		if(MarkupNode = new xml_markup(MarkupType, move(MarkupText)))
		{
			if(!Markup.empty())
			{
				Markup.back()->Children.push_back(MarkupNode);
			}

			Markup.push_back(MarkupNode);
		}
	}

	inline void xml_source::SwitchState(xml_parsing_states NextState)
	{
		if(ParsingState != NextState)
		{
			LastParsingState = ParsingState;
			ParsingState = NextState;
		}
	}

	inline bool xml_source::StateMatches(xml_parsing_states XMLParsingState)
	{
		return ParsingState == XMLParsingState;
	}

	/****
	***** MAIN SOURCE PARSING METHOD
	***** PARSES XML FILES OF COURSE
	****/
	void xml_source::ProcessState()
	{
		char DebugChar = Buffer()->sgetc();

		switch(ParsingState)
		{
			case xml_parsing_states::ParsingContent:
			{
				// if(TryToParseContent())

				break;
			}

			case xml_parsing_states::ParsingAtts:
			{
				size_t Idx = 0;

				while(!Error && TryToParseNameToken('='))
				{
					NameTokens.push_back(WriteBuff);

					Buffer()->sbumpc();

					if(TryToParseLiteral())
					{
						if(xml_markup_attribute* Attribute = new xml_markup_attribute { NameTokens.back(), WriteBuff })
						{
							Markup.back()->Attributes.push_back(Attribute);
						}

						Idx++;
					}

					if(!IsWS())
					{
						SetErrorMissingWS();
						break;
					}

					TrimWS();
				}

				break;
			}

			case xml_parsing_states::ParsingDeclAtts:
			{
				const size_t AttributeCount = 3;

				size_t VersionIdx = 0;
				size_t EncodingIdx = 0;
				size_t StandaloneIdx = 0;
				size_t Idx = 0;

				while(!Error && Idx < AttributeCount && TryToParseNameToken('='))
				{
					NameTokens.push_back(WriteBuff);

					Buffer()->sbumpc();

					if(TryToParseLiteral())
					{
						if(xml_markup_attribute* Attribute = new xml_markup_attribute { NameTokens.back(), WriteBuff })
						{
							if(NameTokens.back() == XMLDeclVersionAttribute)
							{
								VersionIdx = Idx;
							}
							else if(NameTokens.back() == XMLDeclEncodingAttribute)
							{
								EncodingIdx = Idx;
							}
							else if(NameTokens.back() == XMLDeclStandaloneAttribute)
							{
								StandaloneIdx = Idx;
							}

							Markup.back()->Attributes.push_back(Attribute);
						}

						Idx++;
					}

					if(!IsWS())
					{
						SetErrorMissingWS();
						break;
					}

					TrimWS();
				}

				// final error checking
				if(VersionIdx != 0)
				{
					SetErrorOutOfOrderDeclAttribute(VersionIdx, XMLDeclVersionAttributeIndex);
				}

				if(EncodingIdx != 1)
				{
					SetErrorOutOfOrderDeclAttribute(EncodingIdx, XMLDeclEncodingAttributeIndex);
				}

				if(StandaloneIdx != 2)
				{
					SetErrorOutOfOrderDeclAttribute(StandaloneIdx, XMLDeclStandaloneAttributeIndex);
				}

				SwitchState(xml_parsing_states::ParsingEndTag);
				break;
			}
		}
	}

	void xml_source::LexBuff()
	{
		if(!Error)
		{
			char DebugChar = Buffer()->sgetc();

			switch(Buffer()->sgetc())
			{
				case '>':
				{
					// expected character
					// in stream;
					break;
				}

				/****
				***** PARSE XML STARTING ELEMENT
				****/
				case '<':
				{
					// we can lex this character in these 
					// scenarios
					if(StateMatches(xml_parsing_states::ParsingContent) ||
					   StateMatches(xml_parsing_states::ParsingStartTag))
					{
						if(StateMatches(xml_parsing_states::ParsingContent))
						{
							SwitchState(xml_parsing_states::ParsingStartTag);
						}	

						if(Markup.empty())
						{
							if(TryToParseDeclStart())
							{
								PushMarkup(xml_markup_types::DocDeclTag,
										   XMLDeclStartTag.data());
								break;
							}
						}
						else
						{
							if(TryToParseTypeStart())
							{
								PushMarkup(xml_markup_types::DocTypeTag,
										   XMLDocTypeStartTag.data());
								break;
							}
							else if(TryToParseCommentStart())
							{
								PushMarkup(xml_markup_types::CommentTag, 
										   CommentStartTag.data());
								break;
							}
							else
							{
								// advance input buffer
								// fewer bytes to read;
								Buffer()->sbumpc();

								DebugChar = Buffer()->sgetc();

								if(TryToParseNameToken('>'))
								{
									Buffer()->sbumpc();

									PushMarkup(xml_markup_types::UserTag, move(WriteBuff));
									SwitchState(xml_parsing_states::ParsingContent);
								}
								else
								{
									if(!Error)
									{
										PushMarkup(xml_markup_types::UserTag, move(WriteBuff));
										SwitchState(xml_parsing_states::ParsingAtts);
									}
								}
							}
						}
					}

					break;
				}
				case '?':
				{
					if(StateMatches(xml_parsing_states::ParsingEndTag) && !TryToParseDeclEnd())
					{
						SetErrorMalformedDeclTag();

						break;
					}

					// set end tag
					Markup.back()->EndTag = XMLDeclEndTag.data();

					break;
				}
				case '-':
				{
					if(StateMatches(xml_parsing_states::ParsingEndTag) && !TryToParseCommentEnd())
					{
						SetErrorMalformedCommentTag();

						break;
					}

					// set end tag
					Markup.back()->EndTag = CommentEndTag.data();

					break;
				}
				
			}
		}
	}

	xml_document* xml_source::Parse()
	{
		xml_document* Doc = nullptr;

		while(Buffer()->in_avail() > 0 && !Error)
		{
			char DebugChar = Buffer()->sgetc();

			if(!StateMatches(xml_parsing_states::ParsingContent))
			{
				TrimWS();
			}

			DebugChar = Buffer()->sgetc();

			ProcessState();
			LexBuff();
		}

		return Doc;
	}

	/****
	***** PRIVATE PARSING METHODS
	****/
	inline bool xml_source::SetErrorState()
	{
		if(!Error)
		{
			Error = true;
			return Error;
		}

		return !Error;
	}

	inline void xml_source::SetErrorMalformedNameToken()
	{
		if(SetErrorState())
		{
			ReadBuff(BytesRead);

			ErrorBuff << "received malformed name token: "
					  << ExtraStringBuff << "\n";

			ExtraStringBuff.clear();
		}
	}

	inline void xml_source::SetErrorMalformedStartTag()
	{
		if(SetErrorState())
		{
			ReadBuff(BytesRead);

			ErrorBuff << "received malformed start tag: "
					  << ExtraStringBuff << "\n";

			ExtraStringBuff.clear();
		}
	}

	inline void xml_source::SetErrorMissingEndTag(string ExpectedText)
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

	inline void xml_source::SetErrorMissingXMLVersionAttribute()
	{
		if(SetErrorState())
		{
			ErrorBuff << "error missing xml version attribute"
					  << endl;
		}
	}		

	inline void xml_source::SetErrorOutOfOrderDeclAttribute(int ReceivedOrder, int ExpectedOrder)
	{
		if(SetErrorState())
		{
			ErrorBuff << "out of order decl attribute"
					  << endl;
		}
	}

	inline void xml_source::SetErrorFileNotOpen(const char* XMLSourceDiskPath)
	{
		if(SetErrorState())
		{
			ErrorBuff << "failed to open file " 
				      << XMLSourceDiskPath << endl;
		}
	}

	inline void xml_source::SetErrorFailedToReadFile(const char* XMLSourceDiskPath)
	{
		if(SetErrorState())
		{
			ErrorBuff << "failed to read file " 
				      << XMLSourceDiskPath << endl;
		}
	}

	inline void xml_source::SetErrorMissingQuotes()
	{
		if(SetErrorState())
		{
			ErrorBuff << "[ERROR] literal value missing quotes"
					  << endl;
		}
	}

	inline void xml_source::SetErrorUnclosedTag()
	{
		if(SetErrorState())
		{
			ErrorBuff << "[ERROR] unclosed xml tag"
					  << endl;
		}
	}

	inline void xml_source::SetErrorMissingAttVal()
	{
		if(SetErrorState())
		{
			ErrorBuff << "[ERROR] missing attribute value"
					  << endl;
		}
	}

	inline void xml_source::SetErrorIllegalNameStart()
	{
		if(SetErrorState())
		{
			ErrorBuff << "[ERROR] illegal name start char\n"
					  << "        received " << Buffer()->sgetc() << endl;
		}
	}

	inline void xml_source::SetErrorIllegalLiteralVal()
	{
		if(SetErrorState())
		{ 
			ErrorBuff << "[ERROR] illegal literal value\n";
			ErrorBuff << "        received " << Buffer()->sgetc() << endl;
		}
	}

	inline void xml_source::SetErrorMissingWS()
	{
		if(SetErrorState())
		{ 
			ErrorBuff << "[ERROR] missing required "
				      << "whitespace\n";
		}
	}

	inline void xml_source::SetErrorMalformedTypeTag()
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

	inline void xml_source::SetErrorMalformedDeclTag()
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

	inline void xml_source::SetErrorMalformedCommentTag()
	{
		if(SetErrorState())
		{
			ErrorBuff << "[ERROR] malformed comment tag\n"
					  << endl;
		}
	}

	inline void xml_source::SetErrorEncounteredInvalidStartTag()
	{
		if(SetErrorState())
		{
			ReadBuff(BytesRead);

			ErrorBuff << "[ERROR] invalid start tag encountered\n"
					  << "        START TAG" << ExtraStringBuff 
					  << endl;
		}
	}

	inline void xml_source::SetErrorMissingAttribVal()
	{
		if(SetErrorState())
		{
			ErrorBuff << "[ERROR] missing attrib val\n"
					  << endl;
		}
	}

	inline void xml_source::Rewind(size_t Count)
	{
		while(Count > 0)
		{
			// add bytes back
			// to the input buffer
			Buffer()->sungetc();

			Count--;
		}
	}

	inline string xml_source::GetLastError()
	{
		return ErrorBuff.str();
	}

	inline void xml_source::OutputLastError()
	{
		string ErrorMessage = GetLastError();

		if(!ErrorMessage.empty())
		{
			cout << ErrorMessage 
				 << endl;
		}
	}

	inline streambuf* xml_source::Buffer()
	{
		return SourceBuff.rdbuf();
	}

	void xml_source::ReadBuff(size_t Count)
	{
		size_t BytesToRead = Count;

		if(BytesToRead > Buffer()->in_avail())
		{
			BytesToRead = Buffer()->in_avail();
		}

		if(!ExtraStringBuff.empty())
		{
			ExtraStringBuff.clear();
		}

		ExtraStringBuff.resize(BytesToRead);

		Buffer()->sgetn(ExtraStringBuff.data(), BytesToRead);
	}

    inline bool xml_source::IsNL()
    {
        return (Buffer()->sgetc() == '\r' ||
                Buffer()->sgetc() == '\n');
    }

    inline bool xml_source::IsWS()
    {
        return (Buffer()->sgetc() == ' '  ||
		        Buffer()->sgetc() == '\t' ||
                Buffer()->sgetc() == '\r' ||
                Buffer()->sgetc() == '\n');
    }

    inline void xml_source::TrimWS()
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

	inline bool xml_source::Match(const char* Bytes, size_t ByteCount)
	{
		if(ByteCount > Buffer()->in_avail())
		{
			return false;
		}

		BytesRead = 0;

		bool Matched = true;

		for(size_t Idx = 0; Idx < ByteCount; Idx++)
		{
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

	inline bool xml_source::TryToParseContent()
	{
		bool Success = true;

		WriteBuff.clear();

		BytesRead = 0;

		while(Buffer()->in_avail() > 0)
		{
			if(Buffer()->sgetc() == '<' ||
			   Buffer()->sgetc() == '&')
			{
				Success = false;
				break;
			}

			Buffer()->sbumpc();
			BytesRead++;
		}

		WriteBuff.resize(BytesRead);

		Buffer()->sgetn(WriteBuff.data(), 
						WriteBuff.size());

		ContentBuff = WriteBuff;

		return Success;
	}

	inline bool xml_source::TryToParseEndTag(string TagText)
	{
		if(Match(TagText.data(),
				 TagText.size()))
		{
			if(!IsWS())
			{
				// grab last byte
				// read for error 
				// reporting
				BytesRead++;

				return false;
			}

			return true;
		}

		return false;
	}

	inline bool xml_source::TryToParseStartTag(string TagText)
	{
		if(Match(TagText.c_str(),
			     TagText.size()))
		{
			if(!IsWS())
			{
				// grab last byte
				// read for error 
				// reporting
				BytesRead++;

				return false;
			}

			return true;
		}

		return false;
	}

	inline bool xml_source::TryToParseDeclEnd()
	{
		if(!TryToParseEndTag(XMLDeclEndTag.data()))
		{
			Rewind(BytesRead);

			return false;
		}

		SwitchState(xml_parsing_states::ParsingStartTag);

		return true;
	}

	inline bool xml_source::TryToParseDeclStart()
	{		
		if(!TryToParseStartTag(XMLDeclStartTag.data()))
		{
			Rewind(BytesRead);

			return false;
		}

		SwitchState(xml_parsing_states::ParsingDeclAtts);

		return true;
	}

	inline bool xml_source::TryToParseTypeEnd()
	{
		if(!TryToParseEndTag(XMLDocTypeEndTag.data()))
		{
			Rewind(BytesRead);

			return false;
		}

		SwitchState(xml_parsing_states::ParsingStartTag);

		return true;
	}

	inline bool xml_source::TryToParseTypeStart()
	{
		if(!TryToParseStartTag(XMLDocTypeStartTag.data()))
		{
			Rewind(BytesRead);

			return false;
		}

		SwitchState(xml_parsing_states::ParsingTypeAtts);

		return true;
	}

	inline bool xml_source::TryToParseCommentEnd()
	{
		if(!TryToParseEndTag(CommentEndTag.data()))
		{
			Rewind(BytesRead);

			return false;
		}

		SwitchState(xml_parsing_states::ParsingStartTag);

		return true;
	}

	inline bool xml_source::TryToParseCommentStart()
	{
		if(!TryToParseStartTag(CommentStartTag.data()))
		{
			Rewind(BytesRead);

			return false;
		}

		SwitchState(xml_parsing_states::ParsingComment);

		return true;
	}

	inline bool xml_source::TryToParseNameStart()
	{
		/*** [4]		NameStartChar	   :: = ":" | [A - Z] | "_" | [a - z] | [#xC0 - #xD6] | 
		****                                          [#xD8 - #xF6] | [#xF8 - #x2FF] | [#x370 - #x37D] | 
		****                                          [#x37F - #x1FFF] | [#x200C - #x200D] | [#x2070 - #x218F] | 
		****                                          [#x2C00 - #x2FEF] | [#x3001 - #xD7FF] | [#xF900 - #xFDCF] | 
		****                                          [#xFDF0 - #xFFFD] | [#x10000 - #xEFFFF]*/
		const int UpperCaseLower = StaticCast(int, 'A');
		const int UpperCaseHigher = StaticCast(int, 'Z');

		const int LowerCaseLower = StaticCast(int, 'a');
		const int LowerCaseHigher = StaticCast(int, 'z');

		int DebugVal = Buffer()->sgetc();

		bool IsLower = (StaticCast(int, Buffer()->sgetc()) >= LowerCaseLower &&
					    StaticCast(int, Buffer()->sgetc()) <= LowerCaseHigher);
		bool IsUpper = (StaticCast(int, Buffer()->sgetc()) >= UpperCaseLower &&
						StaticCast(int, Buffer()->sgetc()) <= UpperCaseHigher);

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

		WriteBuff.clear();

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
			Rewind(BytesRead);

			WriteBuff.resize(BytesRead);

			Buffer()->sgetn(WriteBuff.data(), 
							WriteBuff.size());
		}
		else
		{
			// maybe an error to rer
			SetErrorMalformedStartTag();

		}

		return ParsedToDelim && !Error;
	}

	inline bool xml_source::TryToParseLiteral()
	{
		if(Buffer()->sgetc() != '\"')
		{
			SetErrorMissingAttribVal();
			return !Error;
		}

		Buffer()->sbumpc();

		WriteBuff.clear();
		BytesRead = 0;

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
			BytesRead++;
		}

		if(Buffer()->sgetc() != '\"')
		{
			SetErrorMissingQuotes();
			return !Error;
		}

		Rewind(BytesRead);

		WriteBuff.resize(BytesRead);

		Buffer()->sgetn(WriteBuff.data(), 
						WriteBuff.size());

		Buffer()->sbumpc();
		return !Error;
	}

	void xml_source::WrapEndTag(string& TagSource, bool Shortened)
	{
		string TempBuff;


	}

	void xml_source::WrapStartTag(string& TagSource)
	{
		string TempBuff("<");
		// TempBuff.append(Source);
		// TempBuff.append(">");


	}
}
