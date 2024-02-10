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
	EndOfBuffer = true;
}

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

	if(!IsWhitespace(true))
	{
		Advance(ByteCount, true);

		if(RewindMirror)
		{
			Rewind(ByteCount);
		}
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

bool xml_reader::TryToParseAttributeValue(bool RewindMirror)
{
	bool Parsed = false;

	if(CompareBytes("\"", 1, RewindMirror))
	{
		while(SelectedByteMirror != BufferEnd)
		{
			if(CompareBytes("\"", 1, RewindMirror))
			{
				Parsed = true;
				break;
			}

			AttributeValueStack[AttributeValueCount].push_back(Extract(SelectedByteMirror));
			SelectedByteMirror++;
		}
	}

	if(Parsed)
	{
		AttributeValueCount++;
	}

	if(RewindMirror)
	{
		SelectedByteMirror = SelectedByte;
	}

	return Parsed;
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

	}

	return EndOfBuffer;
}
