#include "./include/xml_reader.h"

/**
*** INTERNAL HELPER MACROS
*/

#define Extract(SelectedByte)(*(SelectedByte))
#define AtEndOfBuffer(CurSel, BufferEnd)((CurSel) == (BufferEnd))

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

xml_reader::xml_reader(xml_document* XMLDoc)
{
	if(XMLDoc && XMLDoc->IsOpen())
	{
		Doc = XMLDoc;

		BufferSize = XMLDocumentBuffer(Doc).size();
		BufferPos = 0;

		// Buffer Range
		BufferBegin = XMLDocumentBuffer(Doc).data();
		BufferEnd = XMLDocumentBuffer(Doc).data() + BufferSize;

		BytesAvailable = BufferSize;

		// Selected Byte
		SelectedByte = BufferBegin;
		SelectedByteMirror = SelectedByte;

		EndOfBuffer = false;
		Peeking = false;

		if(SelectedByte == BufferEnd)
		{
			EndOfBuffer = true;
		}

		// Current Reading Mode
		Mode = XMLReaderModeEnum(Nothing);

		// set attribute stack count
		AttributeValueCount = 0;

		if(!Read())
		{
			// log

		}
	}
}

xml_reader::~xml_reader()
{
	if(Doc)
	{
		Doc->Close();

		delete Doc;
		Doc = nullptr;
	}

	// Current Reading Mode
	Mode = XMLReaderModeEnum(Nothing);

	// Buffer Range
	BufferBegin = nullptr;
	BufferEnd = nullptr;

	// Selected Byte
	SelectedByte = nullptr;
	SelectedByteMirror = nullptr;

	// Position and Buffer size
	BufferPos = 0;
	BufferSize = 0;
	BytesAvailable = 0;

	// Reading State flags
	Peeking = false;
	EndOfBuffer = true;
}

bool xml_reader::IsWS()
{
	bool WS = false;

	if(!Peeking)
	{
		WS = Extract(SelectedByte) == ' ' ||
			 Extract(SelectedByte) == '\t' ||
			 Extract(SelectedByte) == '\r' ||
			 Extract(SelectedByte) == '\n';
	}
	else
	{
		WS = Extract(SelectedByteMirror) == ' ' ||
			 Extract(SelectedByteMirror) == '\t' ||
		     Extract(SelectedByteMirror) == '\r' ||
			 Extract(SelectedByteMirror) == '\n';
	}

	return WS;
}

void xml_reader::ConsumeWS()
{
	while(!EndOfBuffer && IsWS())
	{
		if(Peeking)
		{
			PeekNext();
		}
		else
		{
			Advance();
		}
	}
}

void xml_reader::Consume()
{
	BytesAvailable = BufferEnd - SelectedByteMirror;
	BufferPos = BufferSize - BytesAvailable;
	SelectedByte = SelectedByteMirror;

}

void xml_reader::PeekNext(size_t ByteCount)
{
	if(BytesAvailable > ByteCount)
	{
		SelectedByteMirror += ByteCount;

		BufferPos += ByteCount;
		BytesAvailable -= ByteCount;
	}
	else
	{
		SelectedByteMirror += BytesAvailable;

		BufferPos += BytesAvailable;
		BytesAvailable = 0;
		EndOfBuffer = true;
	}
}

void xml_reader::Advance(size_t ByteCount)
{
	if(BytesAvailable > ByteCount)
	{
		SelectedByte += ByteCount;
		SelectedByteMirror += ByteCount;

		BufferPos += ByteCount;
		BytesAvailable -= ByteCount;
	}
	else
	{
		SelectedByte += BytesAvailable;
		SelectedByteMirror += BytesAvailable;

		BufferPos += BytesAvailable;
		BytesAvailable = 0;
		EndOfBuffer = true;
	}
}

bool xml_reader::BytesMatch(const char* SrcBytes, size_t ByteCount)
{
	if(ByteCount > BytesAvailable)
	{
		return false;
	}

	const char*& SelectedByteReference = Peeking ? SelectedByteMirror : SelectedByte;

	bool Equal = true;

	for(size_t CharIndex = 0; CharIndex < ByteCount; CharIndex++)
	{
		if(SelectedByteReference && Extract(SelectedByteReference) != SrcBytes[CharIndex])
		{
			// we have a problem
			Equal = false;
			break;
		}

		SelectedByteReference++;
	}

	return Equal;
}

bool xml_reader::TryToParseAttributeValue()
{
	bool Parsed = false;

	if(BytesMatch("\"", 1))
	{
		const char*& SelectedByteReference = Peeking ? SelectedByteMirror : SelectedByte;

		while(SelectedByteReference != BufferEnd)
		{
			if(BytesMatch("\"", 1))
			{
				Parsed = true;
				break;
			}

			AttributeValueStack[AttributeValueCount].push_back(Extract(SelectedByteReference));
			SelectedByteReference++;
		}
	}

	if(Parsed)
	{
		AttributeValueCount++;
	}

	return Parsed;
}

bool xml_reader::TryToParseAttribute(const char* Lexeme, size_t ByteCount) 
{
	if(BytesMatch(Lexeme, ByteCount))
	{
		// matched attribute lexeme
		Consume();

		if(BytesMatch("=", 1))
		{
			// matched attribute assignment
			Consume();

			if(TryToParseAttributeValue())
			{
				
				return true;
			}
		}
		else
		{
			// error
			// break;
		}
	}
	else
	{

		return false;
	}
}

bool xml_reader::TryToParseDocumentAttributes()
{
	Peeking = true;

	xml_markup XMLDeclarationMarkup(BuiltinMarkupTagTypeEnum(PrologAndTypeDeclTag), "<?xml", "?>");

	if(BytesMatch(XMLDeclarationMarkup.FirstLexeme.c_str(),
				  XMLDeclarationMarkup.FirstLexeme.size()))
	{
		Consume();

		size_t AttributeCount = XMLDeclarationMarkup.Attributes.size();

		for(size_t Index = 0; Index < AttributeCount; Index++)
		{
			const char* ExpectedBytes = XMLDeclarationMarkup.Attributes[Index].ExpectedBytes.c_str();
			size_t ByteCount = XMLDeclarationMarkup.Attributes[Index].ExpectedBytes.size();

			if(TryToParseAttribute(ExpectedBytes, ByteCount))
			{

			}
		}

		// matched declaration lexeme
		if(BytesMatch(XMLDeclarationMarkup.LastLexeme.c_str(), 
					  XMLDeclarationMarkup.LastLexeme.size()))
		{

			return true;
		}
		else
		{
			// log error & 
			// report error
		}
	}
	else
	{
		// log error & 
		// report error

		return false;
	}
}



void xml_reader::PopAttributeValueStack()
{
	if(AttributeValueCount > 0)
	{
		AttributeValueStack[AttributeValueCount - 1].clear();

		AttributeValueCount--;
	}
}

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

bool xml_reader::Read()
{
	if(Doc && Doc->IsOpen())
	{
		if(TryToParseDocumentAttributes())
		{
			// Doc->SetDocAttribute

		}
	}

	return EndOfBuffer;
}
