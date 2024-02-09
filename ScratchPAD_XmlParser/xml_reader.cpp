#include "./include/xml_reader.h"

/**
*** XML_DOC
*/

bool xml_doc::Open(const char* Path)
{
	bool OpenResult = false;

	FileHandle = OpenForReading(Path);

	if(FileHandle)
	{
		OpenResult = true;
	}

	return OpenResult;
}

bool xml_doc::IsOpen() const
{
	return FileHandle->_IsOpen;
}

void xml_doc::Close()
{
	CloseFile(FileHandle);
	FileHandle = nullptr;
}

void xml_doc::SetDocDeclAttribute(xml_doc_decl_attribute AttributeEnum, string Value)
{
	if(AttributeEnum == xml_doc_decl_attribute::DECL_ATTRIBUTE_VERSION)
	{
		DeclAttributes[static_cast<int>(xml_doc_decl_attribute::DECL_ATTRIBUTE_VERSION)] = Value;
	}
	else if(AttributeEnum == xml_doc_decl_attribute::DECL_ATTRIBUTE_ENCODING)
	{
		DeclAttributes[static_cast<int>(xml_doc_decl_attribute::DECL_ATTRIBUTE_ENCODING)] = Value;
	}
	else if(AttributeEnum == xml_doc_decl_attribute::DECL_ATTRIBUTE_STANDALONEDECL)
	{
		DeclAttributes[static_cast<int>(xml_doc_decl_attribute::DECL_ATTRIBUTE_STANDALONEDECL)] = Value;
	}
}

xml_doc* xml_doc::CreateAndOpen(const char* Path)
{
	xml_doc* Doc = new xml_doc();

	if(!Doc)
	{
		return nullptr;
	}

	if(!Doc->Open(Path))
	{
		delete Doc;
		Doc = nullptr;
	}

	return Doc;
}

xml_doc* xml_doc::Create(const char* Path)
{
	xml_doc* Doc = new xml_doc();

	if(!Doc)
	{
		return nullptr;
	}

	return Doc;
}

/**
*** XML_READER
*/

/**
** NOTE REGARDING HOW THIS WORKS
** MY IDEA IS THIS: 
** SelectedByte and SelectedByteMirror will always point to the same memory locations
** EXCEPT when we do a lookahead, in which case SelectedByteMirror will advance past
** the ACTUAL SelectedByte, ANY LOOKAHEAD OPERATION after its complete, should rewind
** the Mirror.
* 
* NOTE TO SELF:
* BY DEFAULT EVERY LOOKAHEAD OPERATION WILL LEAVE THE SelectedByteMirror in
* AN ADVANCED POSITION, meaning SelectedByte will hold a memory address to the memory 
* selected before the look ahead, then when we call Advance(), we advance the SelectedByte to the mirror
*/

bool xml_reader::IsWhitespace(bool LookAhead)
{
	bool Whitespace = false;

	if(!LookAhead)
	{
		Whitespace = Extract(SelectedByte) == ' ' ||
					 Extract(SelectedByte) == '\t' ||
				     Extract(SelectedByte) == '\r' ||
					 Extract(SelectedByte) == '\n';
	}
	else
	{
		Whitespace = Extract(SelectedByteMirror) == ' ' ||
					 Extract(SelectedByteMirror) == '\t' ||
				     Extract(SelectedByteMirror) == '\r' ||
				     Extract(SelectedByteMirror) == '\n';
	}

	return Whitespace;
}

void xml_reader::Rewind(size_t ByteCount)
{
	SelectedByteMirror = SelectedByte;
	BytesAvailable += ByteCount;
}

void xml_reader::LookAhead(size_t ByteCount, bool RewindMirror)
{
	/**
	*** Look Ahead to the next byte
	***
	*/

	Advance(ByteCount, true);

	if(RewindMirror)
	{
		Rewind(ByteCount);
	}
}

void xml_reader::Advance(size_t ByteCount, bool LookAhead)
{
	if(BytesAvailable > ByteCount)
	{
		if(!LookAhead)
		{
			SelectedByte += ByteCount;
			BufferPos += ByteCount;

			SelectedByteMirror = SelectedByte;
			BytesAvailable -= ByteCount;
		}
		else
		{
			SelectedByteMirror += ByteCount;
			BytesAvailable -= ByteCount;
		}
	}
	else
	{
		if(!LookAhead)
		{
			SelectedByte += BytesAvailable;
			BufferPos += BytesAvailable;

			SelectedByteMirror = SelectedByte;
		}
		else
		{
			SelectedByteMirror += BytesAvailable;
			BytesAvailable = 0;
			EndOfBuffer = true;
		}
	}
}

void xml_reader::SkipWS(bool LookAhead)
{
	while(!EndOfBuffer && IsWhitespace(LookAhead))
	{
		Advance(1, LookAhead);
	}
}

bool xml_reader::CompareBytes(const char* SrcBytes, size_t ByteCount, bool RewindMirror)
{
	if(ByteCount > BytesAvailable)
	{
		return false;
	}

	bool Equal = true;

	for(size_t CharIndex = 0; CharIndex < ByteCount; CharIndex++)
	{
		if(SelectedByteMirror && Extract(SelectedByteMirror) != SrcBytes[CharIndex])
		{
			// we have a problem
			Equal = false;
			break;
		}

		SelectedByteMirror++;
	}

	if(RewindMirror)
	{
		SelectedByteMirror = SelectedByte;
	}

	return Equal;
}

bool xml_reader::IsCommentTag(bool RewindMirror)
{
	bool Yes = true;

	Yes = CompareBytes(BuiltinMarkupTag(CommentTag)->StartText, 
					   BuiltinMarkupTag(CommentTag)->StartTextLength, RewindMirror);

	return Yes;
}

bool xml_reader::IsCharDataTag(bool RewindMirror)
{
	bool Yes = true;

	Yes = CompareBytes(BuiltinMarkupTag(ProcessingInstructionTag)->StartText, 
					   BuiltinMarkupTag(ProcessingInstructionTag)->StartTextLength, RewindMirror);

	return Yes;
}

bool xml_reader::IsProcessingInstructionTag(bool RewindMirror)
{
	bool Yes = true;

	Yes = CompareBytes(BuiltinMarkupTag(ProcessingInstructionTag)->StartText, 
					   BuiltinMarkupTag(ProcessingInstructionTag)->StartTextLength, RewindMirror);

	return Yes;
}

bool xml_reader::IsPrologAndTypeDeclTag(bool RewindMirror)
{
	bool Yes = true;

	Yes = CompareBytes(BuiltinMarkupTag(PrologAndTypeDeclTag)->StartText, 
					   BuiltinMarkupTag(PrologAndTypeDeclTag)->StartTextLength, RewindMirror);

	return Yes;
}

bool xml_reader::IsCDataSectionTag(bool RewindMirror)
{
	bool Yes = true;

	Yes = CompareBytes(BuiltinMarkupTag(CDataSectionTag)->StartText, 
					   BuiltinMarkupTag(CDataSectionTag)->StartTextLength, RewindMirror);

	return Yes;
}

bool xml_reader::TryToParseVersionAttribute(bool RewindMirror)
{
	bool Parsed = false;

	Parsed = CompareBytes(BuiltinMarkupTagAttribute(PrologAndTypeDeclTag, XMLVersion).ExpectedBytes,
						  BuiltinMarkupTagAttribute(PrologAndTypeDeclTag, XMLVersion).ByteCount, RewindMirror);

	return Parsed;
}

bool xml_reader::TryToParseDocEncodingAttribute(bool RewindMirror)
{
	bool Parsed = false;

	Parsed = CompareBytes(BuiltinMarkupTagAttribute(PrologAndTypeDeclTag, DocEncoding).ExpectedBytes,
						  BuiltinMarkupTagAttribute(PrologAndTypeDeclTag, DocEncoding).ByteCount, RewindMirror);

	return Parsed;
}

bool xml_reader::TryToParseStandaloneDeclAttribute(bool RewindMirror)
{
	bool Parsed = false;

	Parsed = CompareBytes(BuiltinMarkupTagAttribute(PrologAndTypeDeclTag, StandaloneDecl).ExpectedBytes,
						  BuiltinMarkupTagAttribute(PrologAndTypeDeclTag, StandaloneDecl).ByteCount, RewindMirror);

	return Parsed;
}

bool xml_reader::TryToParseAttributeEquals(bool RewindMirror)
{
	bool Parsed = false;

	Parsed = CompareBytes("=", 1, RewindMirror);

	return Parsed;
}

bool xml_reader::TryToParseQuote(bool RewindMirror)
{
	bool Parsed = false;

	Parsed = CompareBytes("\"", 1, RewindMirror);

	return Parsed;
}

bool xml_reader::TryToParseAttributeValue(bool RewindMirror)
{
	bool Parsed = false;

	if(TryToParseQuote(RewindMirror))
	{
		while(SelectedByteMirror != BufferEnd)
		{
			if(TryToParseQuote(RewindMirror))
			{
				Parsed = true;
				break;
			}

			AttributeValueStack[CurrentAttributeValueCount].push_back(Extract(SelectedByteMirror));
			SelectedByteMirror++;
		}
	}

	if(Parsed)
	{
		CurrentAttributeValueCount++;
	}

	if(RewindMirror)
	{
		SelectedByteMirror = SelectedByte;
	}

	return Parsed;
}

void xml_reader::PopAttributeValueStack()
{
	if(CurrentAttributeValueCount > 0)
	{
		AttributeValueStack[CurrentAttributeValueCount - 1].clear();

		CurrentAttributeValueCount--;
	}
}


bool xml_reader::TryToParsePrologAndTypeDeclTag()
{
	bool Success = false;

	if(IsPrologAndTypeDeclTag())
	{
		// set our new xml reading mode
		Mode = XMLReaderMode(ReadingPrologAndTypeDeclTag);

		// advance xml decl token length
		Advance(BuiltinMarkupTag(PrologAndTypeDeclTag)->StartTextLength);

		// prepare/cleanup for next token/lexeme
		SkipWS();

		for(size_t Index = 0; Index < BuiltinMarkupTag(PrologAndTypeDeclTag)->AttributeCount; Index++)
		{
			xml_builtin_markup_tag_attribute Attribute = BuiltinMarkupTag(PrologAndTypeDeclTag)->Attributes[Index];

			if(CompareBytes(Attribute.ExpectedBytes, Attribute.ByteCount))
			{
				Advance(Attribute.ByteCount);
				SkipWS();

				if(!CompareBytes("=", 1))
				{
					Success = false;
					break;
				}

				Advance();

				SkipWS();

				if(!TryToParseAttributeValue())
				{
					Success = false;
					break;
				}
				else
				{
					Success = true;

					Doc->SetDocDeclAttribute(static_cast<xml_doc_decl_attribute>(Index), AttributeValueStack[CurrentAttributeValueCount - 1]);

					Advance(Doc->DeclAttributes[Index].size() + 2);

					PopAttributeValueStack();
				}
			}
			else
			{
				Success = false;
				break;
			}

			SkipWS();
		}

		if(!CompareBytes("?>", 2))
		{
			Success = false;
		}
		else
		{
			Advance(2);
			SkipWS();
		}
	}

	return Success;
}

bool xml_reader::Read(xml_doc* NewDoc)
{
	if(!NewDoc->IsOpen())
	{
		return false;
	}

	Doc = NewDoc;

	bool ReadResult = true;

	BufferSize = XmlDocBuf(Doc).size();
	BufferBegin = XmlDocBuf(Doc).data();
	BufferEnd = XmlDocBuf(Doc).data() + BufferSize;

	BytesAvailable = BufferSize;

	SelectedByte = BufferBegin;
	SelectedByteMirror = SelectedByte;
	BufferPos = 0;
	EndOfBuffer = false;

	if(SelectedByte == BufferEnd)
	{
		EndOfBuffer = true;
	}

	while(!EndOfBuffer)
	{
		SkipWS();

		switch(Extract(SelectedByte))
		{
			case '<':
			{
				// parsing markup: 
				// start-tags, 
				// end-tags, 
				// empty-element-tags, 
				// entity references, 
				// character references, 
				// comments, 
				// CDATA section delimiters
				// document type declarations
				// processing instructions, 
				// XML declarations,
				// text declarations

				if(TryToParsePrologAndTypeDeclTag())
				{
					cout << "parsed prolog and type decl tag\n";

				}
				else if(IsCommentTag())
				{
					cout << "Found CommentTag\n";
					Mode = XMLReaderMode(ReadingCommentTag);

					Advance(BuiltinMarkupTag(CommentTag)->StartTextLength);
				}
				else if(IsProcessingInstructionTag())
				{
					cout << "Found ProcessingInstructionTag\n";
					Mode = XMLReaderMode(ReadingProcessingInstructionTag);

					Advance(BuiltinMarkupTag(ProcessingInstructionTag)->StartTextLength);
				}
				else if(IsCDataSectionTag())
				{
					cout << "Found CDataSectionTag\n";
					Mode = XMLReaderMode(ReadingCDataSectionTag);

					Advance(BuiltinMarkupTag(CDataSectionTag)->StartTextLength);
				}
				else
				{
					cout << "Found CharDataTag\n";
					Mode = XMLReaderMode(ReadingCharDataTag);
				}

				break;
			}
			case '?':
			{
				if(Mode == XMLReaderMode(ReadingPrologAndTypeDeclTag))
				{
					// i dont think we have to do anything here but set a boolean
					// on the xml doc
				}

				break;
			}
			case '>':
			{
				// the right angle bracket for compatibility reasons must be escaped
				// or a character reference when it appears in a string

				switch(Mode)
				{
					case XMLReaderMode(ReadingCommentTag):
					{
						cout << "Found CommentEndTag\n";
						break;
					}
					case XMLReaderMode(ReadingCharDataTag):
					{
						cout << "Found CharDataEndTag\n";
						break;
					}
					case XMLReaderMode(ReadingProcessingInstructionTag):
					{
						cout << "Found ProcessingInstructionEndTag\n";
						break;
					}
					case XMLReaderMode(ReadingPrologAndTypeDeclTag):
					{
						cout << "Found PrologAndTypeDeclEndTag\n";

						break;
					}
					case XMLReaderMode(ReadingCDataSectionTag):
					{
						cout << "Found CDataSectionEndTag\n";
						break;
					}
				}

				break;
			}

			default:
			{
				switch(Mode)
				{
					case XMLReaderMode(ReadingCommentTag):
					{
						cout << "Found CommentEndTag\n";
						break;
					}
					case XMLReaderMode(ReadingCharDataTag):
					{
						cout << "Found CharDataEndTag\n";
						break;
					}
					case XMLReaderMode(ReadingProcessingInstructionTag):
					{
						cout << "Found ProcessingInstructionEndTag\n";
						break;
					}
					case XMLReaderMode(ReadingPrologAndTypeDeclTag):
					{
										cout << "Found PrologAndTypeDeclEndTag\n";
						break;
					}
					case XMLReaderMode(ReadingCDataSectionTag):
					{
						cout << "Found CDataSectionEndTag\n";
						break;
					}
				}

				break;
			}
		}

		EndOfBuffer = SelectedByte == BufferEnd;
	}

	return ReadResult;
}
