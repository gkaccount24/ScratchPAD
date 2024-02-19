#ifndef XML_SOURCE_H
#define XML_SOURCE_H

/***
*** SCRATCHPAD INCLUDES
***/

#include "xml_document.h"
#include "xml_markup.h"

/***
*** C++ INCLUDES
***/

#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <string>

namespace scratchpad
{
	using std::streambuf;
	using std::stringstream;
	using std::fstream;
	using std::vector;
	using std::string;

	using std::cout;
	using std::endl;

	//enum class xml_parsing_states
	//{
	//	/*****
	//	****** Start - tag
	//	****** [40]   	STag	   :: = '<' Name(S Attribute) * S ? '>'[WFC:Unique Att Spec]
	//	****** [41]   	Attribute	   :: = Name Eq AttValue[VC:Attribute Value Type]
	//	****** [WFC:No External Entity References]
	//	****** [WFC:No < in Attribute Values]
	//	*****/
	//	ParsingStartTag = 0,

	//	/*****
	//	****** ETag	   :: = '</' Name S ? '>'
	//	*****/
	//	ParsingEndTag = 1,

	//	/*****
	//	****** [45]   	elementdecl	   ::=   	'<!ELEMENT' S Name S contentspec S? '>'	[VC: Unique Element Type Declaration]
	//	****** [46]   	contentspec	   ::=   	'EMPTY' | 'ANY' | Mixed | children 
	//	*****/
	//	ParsingElementDecl = 2,

	//	/**** [52]   	AttlistDecl	   :: = '<!ATTLIST' S Name AttDef * S ? '>'
	//	***** [53]   	AttDef	   :: = S Name S AttType S DefaultDecl
	//	****/
	//	ParsingAttributeListDecl = 3,

	//	/****
	//	***** [55]   	StringType	   ::=   	'CDATA'
	//	****/
	//	ParsingStringTypeAtt = 4,

	//	/****
	//	***** [56]   	TokenizedType	   ::=   	'ID'	[VC: ID]
	//	****/
	//	ParsingTokenizedTypeAtt = 5,

	//	/****
	//	***** [57]   	EnumeratedType	   ::=   	NotationType | Enumeration
	//    ***** [58]   	NotationType	   ::=   	'NOTATION' S '(' S? Name (S? '|' S? Name)* S? ')' 	[VC: Notation Attributes]
	//	*****			[VC: One Notation Per Element Type]
	//	*****			[VC: No Notation on Empty Element]
	//	*****			[VC: No Duplicate Tokens]
	//	***** [59]   	Enumeration	   ::=   	'(' S? Nmtoken (S? '|' S? Nmtoken)* S? ')'	[VC: Enumeration]
	//	*****			[VC: No Duplicate Tokens]
	//	****/
	//	ParsingEnumeratedTypeAtt = 6,

	//	/****
	//	***** [56]   	TokenizedType  ::=   	'ID'	[VC: ID]
	//	***** [82]   	NotationDecl   ::=   	'<!NOTATION' S Name S (ExternalID | PublicID) S? '>'	[VC: Unique Notation Name]
	//	***** [83]   	PublicID	   ::=   	'PUBLIC' S PubidLiteral
	//	****/
	//	ParsingNotationDecl = 7,

	//	/****
	//	***** max number of 
	//	***** parsing states
	//	****/
	//	ParsingStateCount = 8
	//};

	enum class xml_parsing_states
	{
		ParsingDecl		  = 0,
		ParsingType		  = 1,
		ParsingComment    = 2,
		ParsingUnknown	  = 3,
		ParsingStateCount = 4 
	};

	class xml_source
	{

	public:
		xml_source(string XMLSourceBuff);
		xml_source(fstream&& XMLSourceFile, 
				   string XMLSourceDiskPath);

		~xml_source();

	public:
		/****
		***** MOVE CONSTRUCTOR FILE 
		***** FOR XML SOURCE
		****/
		xml_source(xml_source&& RSource) noexcept;

	public:
		xml_document* Parse();
		void		  Close();

	private:
		void SetSourceBuff(string XMLSourceBuff);
		bool ReadSourceFile(fstream&& XMLSourceFile, 
							string XMLSourceDiskPath);

		void Rewind(size_t Count);
	private:
		streambuf* Buffer();

		bool IsNL();
		bool IsWS();
		void TrimWS();

		bool Match(const char* Bytes, 
				   size_t ByteCount);

		bool TryToParseNameStart();
		bool TryToParseNameToken(char Delim);
		bool TryToParseLiteral();

		bool TryToParseStartTag(string TagText);
		bool TryToParseDeclStart();
		bool TryToParseTypeStart();
		bool TryToParseCommentStart();

		bool TryToSetParsingDeclState();
		bool TryToSetParsingTypeState();
		bool TryToSetParsingCommentState();

		void Append(const char* Bytes);
		void Append(char Char);

	private:
		/****
		***** XML MARKUP
		***** BUILDER METHODS
		****/
		void PushMarkup(string MarkupText);
		void PopMarkup();

	private: 
		/****
		***** ERROR 
		***** MESSAGES
		****/
		bool SetErrorState();
		void SetErrorIllegalNameStart();
		void SetErrorIllegalLiteralVal();
		void SetErrorMalformedDeclTag();
		void SetErrorMalformedTypeTag();
		void SetErrorMalformedCommentTag();
		void SetErrorMissingAttVal();
		void SetErrorUnclosedTag();
		void SetErrorMissingQuotes();
		void SetErrorMissingAttribVal();
		void SetErrorMissingWS();

		void SetErrorFileNotOpen(const char* XMLSourceDiskPath);
		void SetErrorFailedToReadFile(const char* XMLSourceDiskPath);

		string GetLastError();
		void OutputLastError();

	public:
		/****
		***** SOURCE
		***** STRING
		***** DATA
		****/
		fstream		 SourceFile;
		stringstream SourceBuff;
		size_t		 SourceBuffSize;

		/****
		***** LEXER 
		***** META DATA
		****/
		size_t WSSkipped;
		size_t BytesRead;
		size_t BytesWritten;
		size_t LineCount;
		size_t Row;
		size_t Col;

		/****
		***** PARSING STATE 
		***** DATA
		****/
		xml_parsing_states ParsingState;

		/****
		***** GENERIC STRING
		***** FOR WRITE BUFFERING
		****/
		string		 WriteBuff;

		/****
		***** ERROR
		***** DATA
		****/
		bool		 Error;
		stringstream ErrorBuff;

		/****
		***** NAME
		***** TOKENS
		***** STACK
		****/
		vector<string>	    NameTokens;
		vector<xml_markup*> Markup;
	};
}

#endif