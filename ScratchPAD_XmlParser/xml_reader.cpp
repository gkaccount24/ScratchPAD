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

xml_doc* xml_doc::CreateAndOpen(const char* Path)
{
	xml_doc* Doc = new xml_doc();

	if(!Doc)
	{
		return nullptr;
	}

	if(!Doc->Open(Path))
	{
		return nullptr;
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

bool xml_reader::IsWhitespace() const
{
	bool Whitespace = Extract(SelectedByte) == ' ' ||
					  Extract(SelectedByte) == '\t' ||
					  Extract(SelectedByte) == '\r' ||
					  Extract(SelectedByte) == '\n';

	return Whitespace;
}

void xml_reader::Advance(size_t ByteCount)
{
	if(BytesAvailable > ByteCount)
	{
		SelectedByte += ByteCount;
		BufferPos += ByteCount;

		BytesAvailable -= ByteCount;
	}
	else
	{
		SelectedByte += BytesAvailable;
		BufferPos += BytesAvailable;

		BytesAvailable = 0;

		EndOfBuffer = true;
	}
}

void xml_reader::SkipWS()
{
	while(!EndOfBuffer && IsWhitespace())
	{
		Advance();
	}
}

void xml_reader::CopyByteRange(const char* SrcBytes, char* DstBytes, size_t SrcByteCount)
{
	// NOTHING YET
}

bool xml_reader::CompareBytes(const char* SelectedByteCopy, const char* SrcBytes, size_t ByteCount)
{
	if(ByteCount > BytesAvailable)
	{
		return false;
	}

	bool Equal = true;

	for(size_t CharIndex = 0; CharIndex < ByteCount; CharIndex++)
	{
		if(SelectedByteCopy && Extract(SelectedByteCopy) != SrcBytes[CharIndex])
		{
			// we have a problem
			Equal = false;
			break;
		}

		SelectedByteCopy++;
	}

	return Equal;
}

bool xml_reader::IsCommentTag()
{
	bool Yes = true;

	Yes = CompareBytes(SelectedByte, BuiltinMarkupTag(CommentTag)->StartText, 
									 BuiltinMarkupTag(CommentTag)->StartTextLength);

	return Yes;
}

bool xml_reader::IsCharDataTag()
{
	bool Yes = true;

	Yes = CompareBytes(SelectedByte, BuiltinMarkupTag(ProcessingInstructionTag)->StartText, 
								     BuiltinMarkupTag(ProcessingInstructionTag)->StartTextLength);

	return Yes;
}

bool xml_reader::IsProcessingInstructionTag()
{
	bool Yes = true;

	Yes = CompareBytes(SelectedByte, BuiltinMarkupTag(ProcessingInstructionTag)->StartText, 
									 BuiltinMarkupTag(ProcessingInstructionTag)->StartTextLength);

	return Yes;
}

bool xml_reader::IsPrologAndTypeDeclTag()
{
	bool Yes = true;

	Yes = CompareBytes(SelectedByte, BuiltinMarkupTag(PrologAndTypeDeclTag)->StartText, 
									 BuiltinMarkupTag(PrologAndTypeDeclTag)->StartTextLength);

	return Yes;
}

bool xml_reader::IsCDataSectionTag()
{
	bool Yes = true;

	Yes = CompareBytes(SelectedByte, BuiltinMarkupTag(CDataSectionTag)->StartText, 
									 BuiltinMarkupTag(CDataSectionTag)->StartTextLength);

	return Yes;
}

bool xml_reader::TryToParseVersionAttribute()
{
	bool Parsed = false;

	Parsed = CompareBytes(SelectedByteMirror, BuiltinMarkupTagAttribute(PrologAndTypeDeclTag, XMLVersion).ExpectedBytes,
											  BuiltinMarkupTagAttribute(PrologAndTypeDeclTag, XMLVersion).ByteCount);

	return Parsed;
}

bool xml_reader::TryToParseDocEncodingAttribute()
{
	bool Parsed = false;

	Parsed = CompareBytes(SelectedByteMirror, BuiltinMarkupTagAttribute(PrologAndTypeDeclTag, DocEncoding).ExpectedBytes,
									 		  BuiltinMarkupTagAttribute(PrologAndTypeDeclTag, DocEncoding).ByteCount);

	return Parsed;
}

bool xml_reader::TryToParseStandaloneDeclAttribute()
{
	bool Parsed = false;

	Parsed = CompareBytes(SelectedByteMirror, BuiltinMarkupTagAttribute(PrologAndTypeDeclTag, StandaloneDecl).ExpectedBytes,
											  BuiltinMarkupTagAttribute(PrologAndTypeDeclTag, StandaloneDecl).ByteCount);

	return Parsed;
}

bool xml_reader::TryToParseAttributeEquals()
{
	bool Parsed = false;

	Parsed = CompareBytes(SelectedByteMirror, "=", 1);

	return Parsed;
}

bool xml_reader::TryToParseQuote()
{
	bool Parsed = false;

	Parsed = CompareBytes(SelectedByteMirror, "\"", 1);

	return Parsed;
}

bool xml_reader::TryToParseAttributeValue()
{
	bool Parsed = false;

	string ValueBuffer;

	if(TryToParseQuote())
	{
		size_t BytesAdvanced = 0;

		while(SelectedByteMirror != BufferEnd)
		{
			if(TryToParseQuote())
			{
				Parsed = true;
				break;
			}

			BytesAdvanced++;
			SelectedByteMirror++;
		}
	}

	SelectedByteMirror = SelectedByte;

	return Parsed;
}

bool xml_reader::Read(xml_doc* Doc)
{
	if(!Doc->IsOpen())
	{
		return false;
	}

	bool ReadResult = true;

	BufferSize = XmlDocBuf(Doc).size();
	BufferBegin = XmlDocBuf(Doc).data();
	BufferEnd = XmlDocBuf(Doc).data() + BufferSize;

	BytesAvailable = BufferSize;

	SelectedByte = BufferBegin;
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
				if(IsPrologAndTypeDeclTag())
				{
					cout << "Found PrologAndTypeDeclTag\n";
					Mode = XMLReaderMode(ReadingPrologAndTypeDeclTag);

					Advance(BuiltinMarkupTag(PrologAndTypeDeclTag)->StartTextLength);

					SkipWS();

					// we need to parse the version attribute
					// we need to parse the encoding attribute
					// we need to parse the standalone attribute
					// we need to parse any Name tokens

					if(TryToParseVersionAttribute())
					{
						// parsed xml version number
						Advance(BuiltinMarkupTagAttribute(PrologAndTypeDeclTag, XMLVersion).ByteCount);

						SkipWS();

						if(TryToParseAttributeEquals())
						{
							// parsed equals

							Advance();
							SkipWS();

							if(TryToParseAttributeValue())
							{
								// parsed value

							}
						}
					}

					if(TryToParseDocEncodingAttribute())
					{
						// parsed xml doc encoding
						Advance(BuiltinMarkupTagAttribute(PrologAndTypeDeclTag, DocEncoding).ByteCount);

						SkipWS();

						if(TryToParseAttributeEquals())
						{
							// parsed equals

							Advance();
							SkipWS();

							if(TryToParseAttributeValue())
							{
								// parsed value

							}
						}
					}

					if(TryToParseStandaloneDeclAttribute())
					{
						// parsed xml standalone decl
							
						Advance(BuiltinMarkupTagAttribute(PrologAndTypeDeclTag, StandaloneDecl).ByteCount);

						SkipWS();

						if(TryToParseAttributeEquals())
						{
							// parsed equals

							Advance();
							SkipWS();

							if(TryToParseAttributeValue())
							{
								// parsed value

							}
						}
					}
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
