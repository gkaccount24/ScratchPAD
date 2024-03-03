#include "parser.h"

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

#define EMPTY_STRING_BUFFER ""

namespace scratchpad
{
	namespace xml
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

		parser::parser():
			Source(nullptr),
			NewDocument(nullptr),
			Buffer(nullptr),
			WSSkipped(0), BytesRead(0),
			BytesWritten(0), LineCount(1),
			Row(0), Col(0), 
			ParsingState(xml::parsing_states::ParsingStartTag),
			LastParsingState(xml::parsing_states::ParsingUnknown),
			WriteBuffer(EMPTY_STRING_BUFFER), 
			ExtraStringBuffer(EMPTY_STRING_BUFFER),
			ContentBuffer(EMPTY_STRING_BUFFER), Error(false),
			Diagnostics(xml::diagnostics::ParserDiagnostics())
		{
			/**
			*** PARSER CONSTRUCTOR
			***/
		}

		parser::~parser() 
		{
			/**
			*** PARSER DESTRUCTOR
			***/
		}

		void parser::OutputMessageBuffer(std::ostream& OutputConsole)
		{
			string OutputMessage(move(Diagnostics->MessageBuffer.str()));

			OutputConsole << OutputMessage << "\n" << std::flush;
		}

		void parser::OutputMessageBuffer(std::ofstream& OutputFile)
		{
			string OutputMessage(move(Diagnostics->MessageBuffer.str()));

			OutputFile << OutputMessage << "\n" << std::flush;
		}

		inline bool parser::IsNL()
		{
			return Buffer->sgetc() == '\n';
		}

		inline bool parser::IsWS()
		{
			return (Buffer->sgetc() == ' '  ||
					Buffer->sgetc() == '\t' ||
					Buffer->sgetc() == '\r' ||
					Buffer->sgetc() == '\n');
		}

		inline void parser::TrimWS()
		{
			WSSkipped = 0;

			while(Buffer->in_avail() > 0 && IsWS() && !Error)
			{
		
				// advance input buffer
				// less bytes to read after a bump
				Buffer->sbumpc();
			}
		}

		inline void parser::Close()
		{
			Source->Close();

			NameTokens.clear();
			Markup.clear();

			WSSkipped = 0;
			BytesRead = 0;
			BytesWritten = 0;
			LineCount = 0;
			Row = 0;
			Col = 0;
		}

		void parser::Reset()
		{
			//xml::source* Source;
			//xml::document* NewDocument;
			//vector<xml::document*> ParsedDocuments;
			//streambuf* Buffer;

			//size_t WSSkipped;
			//size_t BytesRead;
			//size_t BytesWritten;
			//size_t LineCount;
			//size_t Row;
			//size_t Col;

			//xml::parsing_states ParsingState;
			//xml::parsing_states LastParsingState;

			//string		 WriteBuffer;
			//string		 ExtraStringBuffer;
			//string		 ContentBuffer;

			//bool		 Error;

			//vector<string>		 NameTokens;
			//vector<xml::markup*> Markup;

			//xml::diagnostics* Diagnostics;

			//xml::document* Document;
		}

		xml::document* parser::Parse(xml::source* XMLSource)
		{
			if(!XMLSource)
			{
				Diagnostics->WriteErrorLog("received nullptr as xml::source*\n");

				return nullptr;
			}

			if(!XMLSource->File.is_open())
			{
				if(!XMLSource->Open(XMLSource->Path))
				{
					Diagnostics->WriteErrorLog("failed to open xml source\n");

					return nullptr;
				}
			}

			if(NewDocument)
			{
				ParsedDocuments.push_back(NewDocument);

				/***
				**** RESET PARSER 
				**** STATE
				***/
				Reset();
			}

			NewDocument = new xml::document;

			if(!NewDocument)
			{
				Diagnostics->WriteErrorLog("failed to allocate memory for xml doc\n");

				return nullptr;
			}

			Source = XMLSource;
			Buffer = XMLSource->Buffer.rdbuf();

			while(Buffer->in_avail() > 0 && !Error)
			{

				Lex();

			}

			if(!NewDocument->RootMarkup)
			{
				delete NewDocument;
				NewDocument = nullptr;
			}

			return NewDocument;
		}

		inline void parser::PopMarkup()
		{
			if(!Markup.empty())
			{
				Markup.pop_back();
			}
		}

		inline void parser::PushMarkup(markup_types MarkupType, string MarkupText)
		{
			markup* MarkupNode = nullptr;

			if(MarkupNode = new markup(MarkupType, move(MarkupText)))
			{
				if(!Markup.empty())
				{
					Markup.back()->Children.push_back(MarkupNode);
				}

				Markup.push_back(MarkupNode);
			}
		}

		inline void parser::SwitchState(parsing_states NextState)
		{
			if(ParsingState != NextState)
			{
				LastParsingState = ParsingState;
				ParsingState = NextState;
			}
		}

		inline bool parser::LastStateMatches(parsing_states XMLParsingState)
		{
			return LastParsingState == XMLParsingState;
		}

		inline bool parser::StateMatches(parsing_states XMLParsingState)
		{
			return ParsingState == XMLParsingState;
		}

		inline void parser::Rewind(size_t Count)
		{
			while(Count > 0)
			{
				// add bytes back
				// to the input buffer
				Buffer->sungetc();

				Count--;
			}
		}

		inline bool parser::Match(const char* Bytes, size_t ByteCount)
		{
			if(ByteCount > Buffer->in_avail())
			{
				return false;
			}

			BytesRead = 0;

			bool Matched = true;

			for(size_t Idx = 0; Idx < ByteCount; Idx++)
			{
				if(Buffer->sgetc() != Bytes[Idx])
				{
					Matched = false;
					break;
				}

				// advance input 
				// buffer
				Buffer->sbumpc();
				BytesRead++;
			}

			return Matched && !Error;
		}

		void parser::ProcessState()
		{
			char DebugChar = Buffer->sgetc();

			switch(ParsingState)
			{
				case parsing_states::ParsingContent:
				{
					// if(TryToParseContent())

					break;
				}

				case parsing_states::ParsingAtts:
				{
					size_t Idx = 0;

					while(!Error && TryToParseNameToken('='))
					{
						NameTokens.push_back(WriteBuffer);

						Buffer->sbumpc();

						if(TryToParseLiteral())
						{
							if(markup_attribute* Attribute = new markup_attribute { NameTokens.back(), WriteBuffer })
							{
								Markup.back()->Attributes.push_back(Attribute);
							}

							Idx++;
						}

						if(!IsWS())
						{
							Diagnostics->SetErrorMissingWS();
							break;
						}

						TrimWS();
					}

					break;
				}

				case parsing_states::ParsingDeclAtts:
				{
					const size_t AttributeCount = 3;

					size_t VersionIdx = 0;
					size_t EncodingIdx = 0;
					size_t StandaloneIdx = 0;
					size_t Idx = 0;

					while(!Error && Idx < AttributeCount && TryToParseNameToken('='))
					{
						NameTokens.push_back(WriteBuffer);

						Buffer->sbumpc();

						if(TryToParseLiteral())
						{
							if(markup_attribute* Attribute = new markup_attribute { NameTokens.back(), WriteBuffer })
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
							Diagnostics->SetErrorMissingWS();
							break;
						}

						TrimWS();
					}

					// final error checking
					if(VersionIdx != 0)
					{
						Diagnostics->SetErrorOutOfOrderDeclAttribute(VersionIdx, XMLDeclVersionAttributeIndex);
					}

					if(EncodingIdx != 1)
					{
						Diagnostics->SetErrorOutOfOrderDeclAttribute(EncodingIdx, XMLDeclEncodingAttributeIndex);
					}

					if(StandaloneIdx != 2)
					{
						Diagnostics->SetErrorOutOfOrderDeclAttribute(StandaloneIdx, XMLDeclStandaloneAttributeIndex);
					}

					SwitchState(parsing_states::ParsingEndTag);
					break;
				}
			}
		}

		void parser::Lex()
		{
			if(!Error)
			{
				switch(Buffer->sgetc())
				{
					case '\t':
					case '\r':
					case '\n':
					case ' ':
					{
						if(IsNL())
						{
							LineCount++;
							Row++;
						}

						WSSkipped++;

						break;
					}

					case '?':
					{
						if(StateMatches(xml::parsing_states::ParsingDeclAtts) &&
						   MarkupTypeMatches(xml::markup_types::DocDeclTag))
						{
							Diagnostics->WriteInfoLog("parsing end tag\n");

							/**
							*** SWITCH STATE
							*** PARSING END TAG
							**/

							SwitchState(xml::parsing_states::ParsingEndTag);
						}
						else
						{
							/**
							*** SET ERROR STATE
							*** UNCOSED START TAG
							**/
	
							Diagnostics->SetErrorUnclosedTag();
						}

						break;
					}

					case '-':
					{
						if(StateMatches(xml::parsing_states::ParsingLiteral)   ||
						   StateMatches(xml::parsing_states::ParsingNameToken) ||
						   StateMatches(xml::parsing_states::ParsingContent))
						{
							Diagnostics->WriteInfoLog("parsing literals, name tokens, tag content\n");

						}

						else if(StateMatches(xml::parsing_states::ParsingComment))
						{
							Diagnostics->WriteInfoLog("parsing comment \n");
							/***
							**** PARSING COMMENT
							***/

							if(!MarkupTypeMatches(xml::markup_types::CommentTag))
							{
								/***
								**** SET ERROR
								**** STATE
								***/
							}
						}

						break;
					}

					case '=':
					{

						if(StateMatches(xml::parsing_states::ParsingDeclAtts))
						{
							if(!MarkupTypeMatches(xml::markup_types::DocDeclTag))
							{
								/**
								**** SetErrorState();
								*** ERROR failed
								**/

								break;
							}

							/**
							*** Parsing Decl attributes
							*** we need to keep track of
							*** attribute index
							**/

							break;
						}
						else if(StateMatches(xml::parsing_states::ParsingAtts))
						{
							/**
							*** Parsing generic 
							*** markup attributes
							**/
						}

						break;
					}

					case '"':
					{
						if(!StateMatches(xml::parsing_states::ParsingLiteral) || 
						   !StateMatches(xml::parsing_states::ParsingContent))
						{
							/***
							**** ERROR ENCOUNTERED
							**** EXPECTED LEXEME
							**/

							break;
						}

						break;
					}

					case '&':
					{
						if(StateMatches(xml::parsing_states::ParsingLiteral) ||
						   StateMatches(xml::parsing_states::ParsingContent))
						{
							Diagnostics->SetErrorState(xml::error_codes::Undefined);

							break;
						}

						/***
						**** ERROR ENCOUNTERED
						**** EXPECTED LEXEME
						**/

						break;
					}

					case '/':
					{
						if(StateMatches(xml::parsing_states::ParsingContent) ||
						   StateMatches(xml::parsing_states::ParsingLiteral) ||
						   StateMatches(xml::parsing_states::ParsingEndTag))
						{
							/**
							*** SET COMMENT END TAG
							**/
						}

						break;
					}

					case '>':
					{
						if(!LastStateMatches(xml::parsing_states::ParsingStartTag) ||
						   !LastStateMatches(xml::parsing_states::ParsingEndTag))
						{
							Diagnostics->WriteErrorLog("unexpected lexeme\n");

							/****
							****** Set Error State
							****** Unexpected Lexeme
							****/

							Diagnostics->SetErrorMalformedStartTag();

							break;
						}

						//SwitchState(xml::parsing_states::ParsingContent);
						//if(!StateMatches(xml::parsing_states::ParsingStartTag))
						//{
						//	/**
						//	*** Unexpected character encountered
						//	*** Maybe report error
						//	**/

						//	break;
						//}

						Diagnostics->WriteInfoLog("parsed start tag\n");

						break;
					}

					/****
					***** PARSE XML STARTING ELEMENT
					****/
					case '<':
					{
						if(StateMatches(xml::parsing_states::ParsingLiteral))
						{
							Diagnostics->SetErrorIllegalLiteralVal(Buffer->sgetc());

							break;
						}

						if(StateMatches(xml::parsing_states::ParsingStartTag))
						{
							Diagnostics->WriteInfoLog("switched state: now parsing start tag");

							/****
							***** SWITCH STATE
							***** PARSE NAMETOKEN
							****/
							SwitchState(xml::parsing_states::ParsingNameToken);
						}
						else if(StateMatches(xml::parsing_states::ParsingContent))
						{	
							Diagnostics->WriteInfoLog("switched state: now parsing end tag");

							/****
							***** SWITCH STATE 
							***** ParsingEndTag
							****/
							SwitchState(xml::parsing_states::ParsingEndTag);
						}

						break;
					}

					default:
					{
						/****
						***** For processing non structural 
						***** markup lexemes
						****/

						break;
					}
				}

				/****
				***** Advance Buffer
				****/
				Buffer->sbumpc();
			}
		}

		inline bool parser::TryToParseContent()
		{
			bool Success = true;

			WriteBuffer.clear();

			BytesRead = 0;

			while(Buffer->in_avail() > 0)
			{
				if(Buffer->sgetc() == '<' ||
				   Buffer->sgetc() == '&')
				{
					Success = false;
					break;
				}

				Buffer->sbumpc();
				BytesRead++;
			}

			WriteBuffer.resize(BytesRead);

			Buffer->sgetn(WriteBuffer.data(), 
							WriteBuffer.size());

			ContentBuffer = WriteBuffer;

			return Success;
		}

		inline bool parser::TryToParseEndTag(string TagText)
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

		inline bool parser::TryToParseStartTag(string TagText)
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

		inline bool parser::TryToParseDeclEnd()
		{
			if(!TryToParseEndTag(XMLDeclEndTag.data()))
			{
				Rewind(BytesRead);

				return false;
			}

			SwitchState(parsing_states::ParsingStartTag);

			return true;
		}

		inline bool parser::TryToParseDeclStart()
		{		
			if(!TryToParseStartTag(XMLDeclStartTag.data()))
			{
				Rewind(BytesRead);

				return false;
			}

			SwitchState(parsing_states::ParsingDeclAtts);

			return true;
		}

		inline bool parser::TryToParseTypeEnd()
		{
			if(!TryToParseEndTag(XMLDocTypeEndTag.data()))
			{
				Rewind(BytesRead);

				return false;
			}

			SwitchState(parsing_states::ParsingStartTag);

			return true;
		}

		inline bool parser::TryToParseTypeStart()
		{
			if(!TryToParseStartTag(XMLDocTypeStartTag.data()))
			{
				Rewind(BytesRead);

				return false;
			}

			SwitchState(parsing_states::ParsingTypeAtts);

			return true;
		}

		inline bool parser::TryToParseCommentEnd()
		{
			if(!TryToParseEndTag(CommentEndTag.data()))
			{
				Rewind(BytesRead);

				return false;
			}

			SwitchState(parsing_states::ParsingStartTag);

			return true;
		}

		inline bool parser::TryToParseCommentStart()
		{
			if(!TryToParseStartTag(CommentStartTag.data()))
			{
				Rewind(BytesRead);

				return false;
			}

			SwitchState(parsing_states::ParsingComment);

			return true;
		}

		inline bool parser::TryToParseNameStart()
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

			int DebugVal = Buffer->sgetc();

			bool IsLower = (StaticCast(int, Buffer->sgetc()) >= LowerCaseLower &&
							StaticCast(int, Buffer->sgetc()) <= LowerCaseHigher);
			bool IsUpper = (StaticCast(int, Buffer->sgetc()) >= UpperCaseLower &&
							StaticCast(int, Buffer->sgetc()) <= UpperCaseHigher);

			bool IsUnderscore = Buffer->sgetc() == '_';

			return(IsLower || IsUpper || IsUnderscore);
		}

		bool parser::TryToParseNameToken(char Delim)
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
				Diagnostics->SetErrorIllegalNameStart(Buffer->sgetc());
				return !Error;
			}

			WriteBuffer.clear();

			// reset byte counter
			// to track name token size
			BytesRead = 0;

			// we need to know what delim
			// we parsed to
			bool ParsedToDelim = false;
			bool ParsedToWS = false;

			while(Buffer->in_avail() > 0)
			{
				if(Buffer->sgetc() == Delim)
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
					Buffer->sbumpc();
				}
			}

			if(ParsedToDelim || ParsedToWS)
			{
				Rewind(BytesRead);

				WriteBuffer.resize(BytesRead);

				Buffer->sgetn(WriteBuffer.data(), 
								WriteBuffer.size());
			}
			else
			{
				// maybe an error to rer
				Diagnostics->SetErrorMalformedStartTag();

			}

			return ParsedToDelim && !Error;
		}

		inline bool parser::TryToParseLiteral()
		{
			if(Buffer->sgetc() != '\"')
			{
				Diagnostics->SetErrorMissingAttribVal();
				return !Error;
			}

			Buffer->sbumpc();

			WriteBuffer.clear();
			BytesRead = 0;

			const char IllegalCharacters[] 
			{
				'<', '&'
			};

			while(Buffer->in_avail() > 0  && 
				  Buffer->sgetc() != '\"' && 
				  !Error)
			{
				if(Buffer->sgetc() == IllegalCharacters[0] && 
				   Buffer->sgetc() == IllegalCharacters[1])
				{
					Diagnostics->SetErrorIllegalLiteralVal(Buffer->sgetc());
					break;
				}

				Buffer->sbumpc();
				BytesRead++;
			}

			if(Buffer->sgetc() != '\"')
			{
				Diagnostics->SetErrorMissingQuotes();
				return !Error;
			}

			Rewind(BytesRead);

			WriteBuffer.resize(BytesRead);

			Buffer->sgetn(WriteBuffer.data(), 
							WriteBuffer.size());

			Buffer->sbumpc();
			return !Error;
		}
	}
}
