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

		parser::parser(xml::source* XMLSource):
			Source(XMLSource)
		{
			//
		}

		parser::parser() { }
		parser::~parser() { }

		inline bool parser::IsNL()
		{
			return (Buffer()->sgetc() == '\r' ||
					Buffer()->sgetc() == '\n');
		}

		inline bool parser::IsWS()
		{
			return (Buffer()->sgetc() == ' '  ||
					Buffer()->sgetc() == '\t' ||
					Buffer()->sgetc() == '\r' ||
					Buffer()->sgetc() == '\n');
		}

		inline void parser::TrimWS()
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

		inline void parser::Close()
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
		}

		document* parser::Parse()
		{
			document* Doc = nullptr;

			while(Buffer()->in_avail() > 0 && !Error)
			{
				char DebugChar = Buffer()->sgetc();

				if(!StateMatches(parsing_states::ParsingContent))
				{
					TrimWS();
				}

				// DebugChar = Buffer()->sgetc();

				ProcessState();
				LexBuff();
			}

			return Doc;
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
				Buffer()->sungetc();

				Count--;
			}
		}

		inline bool parser::Match(const char* Bytes, size_t ByteCount)
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

		void parser::ProcessState()
		{
			char DebugChar = Buffer()->sgetc();

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
						NameTokens.push_back(WriteBuff);

						Buffer()->sbumpc();

						if(TryToParseLiteral())
						{
							if(markup_attribute* Attribute = new markup_attribute { NameTokens.back(), WriteBuff })
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

				case parsing_states::ParsingDeclAtts:
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
							if(markup_attribute* Attribute = new markup_attribute { NameTokens.back(), WriteBuff })
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

					SwitchState(parsing_states::ParsingEndTag);
					break;
				}
			}
		}

		void parser::LexBuff()
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
						if(StateMatches(parsing_states::ParsingContent) ||
						   StateMatches(parsing_states::ParsingStartTag))
						{
							if(StateMatches(parsing_states::ParsingContent))
							{
								SwitchState(parsing_states::ParsingStartTag);
							}	

							if(Markup.empty())
							{
								if(TryToParseDeclStart())
								{
									PushMarkup(markup_types::DocDeclTag, XMLDeclStartTag.data());

									break;
								}
							}
							else
							{
								if(TryToParseTypeStart())
								{
									PushMarkup(markup_types::DocTypeTag, XMLDocTypeStartTag.data());

									break;
								}
								else if(TryToParseCommentStart())
								{
									PushMarkup(markup_types::CommentTag, CommentStartTag.data());

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

										PushMarkup(markup_types::UserTag, move(WriteBuff));
										SwitchState(parsing_states::ParsingContent);
									}
									else
									{
										if(!Error)
										{
											PushMarkup(markup_types::UserTag, move(WriteBuff));
											SwitchState(parsing_states::ParsingAtts);
										}
									}
								}
							}
						}

						break;
					}
					case '?':
					{
						if(StateMatches(parsing_states::ParsingEndTag) && !TryToParseDeclEnd())
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
						if(StateMatches(parsing_states::ParsingEndTag) && !TryToParseCommentEnd())
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

		inline bool parser::TryToParseContent()
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

			int DebugVal = Buffer()->sgetc();

			bool IsLower = (StaticCast(int, Buffer()->sgetc()) >= LowerCaseLower &&
							StaticCast(int, Buffer()->sgetc()) <= LowerCaseHigher);
			bool IsUpper = (StaticCast(int, Buffer()->sgetc()) >= UpperCaseLower &&
							StaticCast(int, Buffer()->sgetc()) <= UpperCaseHigher);

			bool IsUnderscore = Buffer()->sgetc() == '_';

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

		inline bool parser::TryToParseLiteral()
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
	}
}
