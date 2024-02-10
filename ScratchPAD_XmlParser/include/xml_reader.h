#ifndef XML_READER_H
#define XML_READER_H

#include "xml_document.h"
#include "xml_markup.h"

#include <iostream>
#include <string>

using std::cout;
using std::flush;
using std::endl;
using std::string;

/***
**** VARIOUS XML READER MODES
***/

enum class xml_reader_mode
{
	ReadingCommentTag = 0,
	ReadingCharDataTag = 1,
	ReadingProcessingInstructionTag = 2,
	ReadingPrologAndTypeDeclTag = 3,
	ReadingCDataSectionTag = 4,
	Nothing = 5,
	Count = 6
};

#define XMLReaderModeEnum(ReaderModeEnum)(xml_reader_mode::ReaderModeEnum)
#define XMLReaderModeIndex(ReaderModeEnum)((int)XMLReaderModeEnum(ReaderModeEnum))
#define XMLReaderModeEnumFromIndex(ReaderModeEnumIndex)((xml_reader_mode)ReaderModeEnumIndex)

struct xml_reader
{
	// the current document were 
	// suppose to be reading
	xml_document* Doc;

	// Current Reading Mode
	xml_reader_mode Mode;

	// Buffer Range
	const char* BufferBegin;
	const char* BufferEnd;

	// Selected Byte
	const char* SelectedByte;
	const char* SelectedByteMirror;

	// Position and Buffer size
	size_t BufferPos;
	size_t BufferSize;
	size_t BytesAvailable;

	// Reading State flags
	bool EndOfBuffer;

	vector<xml_markup*>			  MarkupNodeStack;
	vector<xml_markup_attribute*> MarkupAttributeStack;

	xml_reader(xml_document* XMLDoc);
	~xml_reader();

	bool Read();
	
	bool IsWS();
	void RemoveWS();
	bool BytesMatch(const char* SrcBytes, size_t ByteCount);

	bool TryToParseDocumentDeclarationMarkup();
	bool TryToParseAttribute(const char* Lexeme, size_t ByteCount);
	bool TryToParseAttributeValue();

	void PopMarkupNodeStack();

	void PushNewMarkup(const char* StartTag, const char* EndTag);
	void PopNewMarkup();

	void PushNewMarkupAttribute(const char* AttributeName);
	void PopNewMarkupAttribute();

private:
	xml_reader(const xml_reader& Rhs) = delete;
	xml_reader(xml_reader&& Rhs) = delete;

};

#endif
