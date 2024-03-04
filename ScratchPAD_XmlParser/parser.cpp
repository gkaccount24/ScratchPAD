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

using std::isalpha;
using std::isdigit;

namespace scratchpad
{
	namespace xml
	{
		static const string_view XMLDeclStartTag = "xml";

		static const string_view XMLDeclStartTagWrapped = "<?xml";
		static const string_view XMLDeclEndTagWrapped = "?>";

		static const string_view XMLDocTypeStartTag = "DOCTYPE";
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
				// Reset();
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
				switch(Buffer->sgetc())
				{
					case '\t':
					case '\r':
					case '\n':
					case ' ':
					{

						if(StateMatches(xml::parsing_states::ParsingNameToken))
						{
							/***
							**** MATCH NAME TOKEN AGAINST
							**** EXPECTED NAME TOKEN
							***/

							if(BytesRead == NextExpectedToken.size())
							{
								if(WriteBuffer != NextExpectedToken)
								{
									Diagnostics->SetErrorState(xml::error_codes::Undefined);

									/**
									*** SET ERROR STATE
									**/
								}

								BytesRead = 0;
								WriteBuffer.clear();

								PushMarkup(xml::markup_types::DocDeclTag, 
										   XMLDeclStartTagWrapped.data());
							}
						}

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
						WSSkipped = 0;

						if(LastStateMatches(xml::parsing_states::ParsingStartTag) &&
						   StateMatches(xml::parsing_states::ParsingNameToken))
						{
							/****
							***** SWITCH STATE
							***** Parsing START TAG
							***** NEEDS TO MATCH ExpectedToken
							****/

							NextExpectedToken = XMLDeclStartTag;
							BytesRead = 0;
							WriteBuffer.clear();
						}
						//else if(StateMatches(xml::parsing_states::ParsingDeclAtts) &&
								//MarkupTypeMatches(xml::markup_types::DocDeclTag))
						{
							//Diagnostics->WriteInfoLog("parsing end tag\n");

							/**
							*** SWITCH STATE
							*** PARSING END TAG
							**/

							//SwitchState(xml::parsing_states::ParsingEndTag);
						}
						//else
						{
							/**
							*** SET ERROR STATE
							*** UNCOSED START TAG
							**/
	
							// Diagnostics->SetErrorUnclosedTag();
						}

						break;
					}

					case '-':
					{
						WSSkipped = 0;

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
						WSSkipped = 0;

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
						WSSkipped = 0;

						if(!StateMatches(xml::parsing_states::ParsingLiteral) || 
						   !StateMatches(xml::parsing_states::ParsingContent))
						{
							/***
							**** ERROR ENCOUNTERED
							**** EXPECTED LEXEME
							**/

							Diagnostics->SetErrorState(xml::error_codes::Undefined);

							break;
						}

						break;
					}

					case '&':
					{
						WSSkipped = 0;

						if(StateMatches(xml::parsing_states::ParsingLiteral) ||
						   StateMatches(xml::parsing_states::ParsingContent))
						{
							/***
							**** ERROR ENCOUNTERED
							**** EXPECTED LEXEME
							**/

							Diagnostics->SetErrorState(xml::error_codes::Undefined);

							break;
						}

						break;
					}

					case '/':
					{
						WSSkipped = 0;

						if(!StateMatches(xml::parsing_states::ParsingContent) ||
						   !StateMatches(xml::parsing_states::ParsingLiteral) ||
						   !StateMatches(xml::parsing_states::ParsingEndTag))
						{
							/**
							*** SET COMMENT END TAG
							**/

							Diagnostics->SetErrorState(xml::error_codes::Undefined);

							break;
						}

						break;
					}

					case '>':
					{
						WSSkipped = 0;

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

						Diagnostics->WriteInfoLog("parsed start tag\n");

						break;
					}

					/****
					***** PARSE XML STARTING ELEMENT
					****/
					case '<':
					{
						WSSkipped = 0;

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
						WSSkipped = 0;

						/****
						***** For processing non structural 
						***** markup lexemes
						****/

						if(StateMatches(xml::parsing_states::ParsingNameToken))
						{
							Diagnostics->WriteInfoLog("scanning name token...\n");

							if(BytesRead == 0)
							{
								Diagnostics->WriteInfoLog("parsing name token start\n");

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

								bool IsValid = (IsLower || IsUpper || IsUnderscore);

								if(!IsValid)
								{
									Diagnostics->SetErrorIllegalNameStart(Buffer->sgetc());
								}

								/****
								***** APPEND TO WRITE BUFFER
								****/
								WriteBuffer += Buffer->sgetc();
								BytesRead++;
							}
							else
							{
								if(!isalpha(Buffer->sgetc()) &&
								   !isdigit(Buffer->sgetc()))
								{
									/***
									**** SET ERROR STATE
									***/

									char Cstr = Buffer->sgetc();

									Diagnostics->SetErrorState(xml::error_codes::Undefined);
								}
								else
								{
									WriteBuffer += Buffer->sgetc();
									BytesRead++;
								}
							}
						}

						break;
					}
				}

				/****
				***** Advance Buffer
				****/
				Buffer->sbumpc();
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

		bool parser::MarkupTypeMatches(xml::markup_types MarkupType)
		{
			return Markup.back()->Type == MarkupType;
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

						if(StateMatches(xml::parsing_states::ParsingNameToken))
						{
							/***
							**** MATCH NAME TOKEN AGAINST
							**** EXPECTED NAME TOKEN
							***/

							if(BytesRead > 0)
							{
								if(WriteBuffer != NextExpectedToken)
								{
									Diagnostics->SetErrorState(xml::error_codes::Undefined);

									/**
									*** SET ERROR STATE
									**/
								}

								PushMarkup(xml::markup_types::DocDeclTag, XMLDeclStartTag.data());
							}

							BytesRead = 0;
							WriteBuffer.clear();
						}

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
						WSSkipped = 0;

						if(LastStateMatches(xml::parsing_states::ParsingStartTag) &&
						   StateMatches(xml::parsing_states::ParsingNameToken))
						{
							/****
							***** SWITCH STATE
							***** Parsing START TAG
							***** NEEDS TO MATCH ExpectedToken
							****/

							NextExpectedToken = XMLDeclStartTag;
							BytesRead = 0;
							WriteBuffer.clear();
						}
						//else if(StateMatches(xml::parsing_states::ParsingDeclAtts) &&
								//MarkupTypeMatches(xml::markup_types::DocDeclTag))
						{
							//Diagnostics->WriteInfoLog("parsing end tag\n");

							/**
							*** SWITCH STATE
							*** PARSING END TAG
							**/

							//SwitchState(xml::parsing_states::ParsingEndTag);
						}
						//else
						{
							/**
							*** SET ERROR STATE
							*** UNCOSED START TAG
							**/
	
							// Diagnostics->SetErrorUnclosedTag();
						}

						break;
					}

					case '-':
					{
						WSSkipped = 0;

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
						WSSkipped = 0;

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
						WSSkipped = 0;

						if(!StateMatches(xml::parsing_states::ParsingLiteral) || 
						   !StateMatches(xml::parsing_states::ParsingContent))
						{
							/***
							**** ERROR ENCOUNTERED
							**** EXPECTED LEXEME
							**/

							Diagnostics->SetErrorState(xml::error_codes::Undefined);

							break;
						}

						break;
					}

					case '&':
					{
						WSSkipped = 0;

						if(StateMatches(xml::parsing_states::ParsingLiteral) ||
						   StateMatches(xml::parsing_states::ParsingContent))
						{
							/***
							**** ERROR ENCOUNTERED
							**** EXPECTED LEXEME
							**/

							Diagnostics->SetErrorState(xml::error_codes::Undefined);

							break;
						}

						break;
					}

					case '/':
					{
						WSSkipped = 0;

						if(!StateMatches(xml::parsing_states::ParsingContent) ||
						   !StateMatches(xml::parsing_states::ParsingLiteral) ||
						   !StateMatches(xml::parsing_states::ParsingEndTag))
						{
							/**
							*** SET COMMENT END TAG
							**/

							Diagnostics->SetErrorState(xml::error_codes::Undefined);

							break;
						}

						break;
					}

					case '>':
					{
						WSSkipped = 0;

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

						Diagnostics->WriteInfoLog("parsed start tag\n");

						break;
					}

					/****
					***** PARSE XML STARTING ELEMENT
					****/
					case '<':
					{
						WSSkipped = 0;

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
						WSSkipped = 0;

						/****
						***** For processing non structural 
						***** markup lexemes
						****/

						if(StateMatches(xml::parsing_states::ParsingNameToken))
						{
							Diagnostics->WriteInfoLog("scanning name token...\n");

							if(BytesRead == 0)
							{
								Diagnostics->WriteInfoLog("parsing name token start\n");

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

								bool IsValid = (IsLower || IsUpper || IsUnderscore);

								if(!IsValid)
								{
									Diagnostics->SetErrorIllegalNameStart(Buffer->sgetc());
								}

								/****
								***** APPEND TO WRITE BUFFER
								****/
								WriteBuffer += Buffer->sgetc();
								BytesRead++;
							}
							else
							{
								if(!isalpha(Buffer->sgetc()) ||
								   !isdigit(Buffer->sgetc()))
								{
									/***
									**** SET ERROR STATE
									***/

									Diagnostics->SetErrorState(xml::error_codes::Undefined);
								}
								else
								{
									WriteBuffer += Buffer->sgetc();
									BytesRead++;
								}
							}
						}

						break;
					}
				}

				/****
				***** Advance Buffer
				****/
				Buffer->sbumpc();
			}
		}
	}
}
