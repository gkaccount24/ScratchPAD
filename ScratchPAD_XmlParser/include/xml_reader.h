#ifndef XML_READER_H
#define XML_READER_H

#include "xml_document.h"
#include "xml_markup.h"

#include <iostream>
#include <algorithm>
#include <string>

using std::reverse;
using std::cout;
using std::flush;
using std::endl;
using std::string;

struct xml_name_token
{
	string Name;

	static xml_name_token* Create(const char* TokenText, size_t ByteCount)
	{
		string Text(TokenText, ByteCount);

		if(!Text.empty())
		{
			xml_name_token* Token = nullptr;

			if(Token = new xml_name_token())
			{
				Token->Name = move(Text);
			}

			return Token;
		}

		return nullptr;
	}
};

/***
**** VARIOUS XML READER MODES
***/

struct xml_reader
{
	// the current document were 
	// suppose to be reading
	xml_document* Doc;

	// Current Reading Mode
	// xml_reader_mode Mode;

	// Buffer Range
	const char* BufferBegin;
	const char* BufferEnd;

	// Selected Byte
	const char* SelectedByte;
	const char* SelectedByteMirror;

	// Position and Buffer size
	size_t WSSkipped;
	size_t BufferPos;
	size_t BufferSize;
	size_t BytesAvailable;

	// Reading State flags
	bool EndOfBuffer;
	string CharDataBuf;

	vector<xml_name_token*> NameTokenStack;
	vector<xml_markup*> MarkupStack;
	vector<xml_markup_attribute*> MarkupAttributeStack;

	xml_reader(xml_document* XMLDoc);
	~xml_reader();

	bool BytesMatch(const char* SrcBytes, size_t ByteCount);

	bool TryToParseDocumentDeclarationMarkup();
	bool TryToParseNameToken(char EndDelimiter, bool EatInitialByte = true);
	bool TryToParseAttribute(const char* Lexeme, size_t ByteCount);
	bool TryToParseAttributeValue();

	void PushMarkupAttribute(const char* AttributeName, size_t ByteCount);
	void PushMarkup(const char* StartTag, const char* EndTag);
	void PushMarkup(const char* NameToken, size_t Length);
	void PopMarkup();

	void RemoveWS();
	bool IsWS();

	bool Read();

private:
	xml_reader(const xml_reader& Rhs) = delete;
	xml_reader(xml_reader&& Rhs) = delete;

};

#endif
