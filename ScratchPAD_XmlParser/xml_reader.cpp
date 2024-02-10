#include "./include/xml_reader.h"

/**
*** INTERNAL HELPER MACROS
*/

#define Extract(SelectedByte)(*(SelectedByte))
#define AtEndOfBuffer(CurSel, BufferEnd)((CurSel) == (BufferEnd))

#define MarkupDOM(NodeStack)(NodeStack[NodeStack.size() - 1])
#define PushMarkupNode(NodeStack, Node)(NodeStack.push_back(Node))

/**
*** XML_READER
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

		AttributeValueStack.resize(128);
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

	AttributeValueStack.clear();
	AttributeValueStack.shrink_to_fit();
	AttributeValueCount = 0;
}

bool xml_reader::IsWS()
{
	bool WS = false;

	WS = Extract(SelectedByteMirror) == ' ' ||
		 Extract(SelectedByteMirror) == '\t' ||
		 Extract(SelectedByteMirror) == '\r' ||
		 Extract(SelectedByteMirror) == '\n';

	return WS;
}

void xml_reader::RemoveWS()
{
	while(SelectedByteMirror != BufferEnd && IsWS())
	{
		SelectedByte++;
		SelectedByteMirror++;
		BufferPos++;
		BytesAvailable--;
	}
}

bool xml_reader::BytesMatch(const char* SrcBytes, size_t ByteCount)
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

	if(Equal)
	{
		SelectedByte = SelectedByteMirror;
		BufferPos += ByteCount;
		BytesAvailable -= ByteCount;

		return true;
	}
	else
	{
		// error, lexeme immediatelly
		// following our tag has to be ws
		return false;
	}
}

bool xml_reader::TryToParseAttributeValue()
{
	assert(!MarkupNodeStack.empty());

	bool Parsed = false;
	size_t TempByteCount = 0;

	if(Extract(SelectedByteMirror) == '\"')
	{		
		SelectedByteMirror++;
		TempByteCount++;
		
		while(SelectedByteMirror != BufferEnd)
		{
			if(Extract(SelectedByteMirror) == '\"')
			{
				SelectedByteMirror++;
				TempByteCount++;

				Parsed = true;
				break;
			}

			// size_t NodeStackIndex = MarkupDOM(MarkupNodeStack)->Attributes.size() - 1;
			// MarkupDOM(MarkupNodeStack)->Attributes[NodeStackIndex].push_back(new xml_markup());
			// AttributeValueStack[AttributeValueCount].push_back(Extract(SelectedByteMirror));

			SelectedByteMirror++;
			TempByteCount++;
		}

		if(Parsed)
		{
			AttributeValueCount++;

			SelectedByte = SelectedByteMirror;
			BufferPos += TempByteCount;
			BytesAvailable -= TempByteCount;
		}
	}

	return Parsed;
}

bool xml_reader::TryToParseAttribute(const char* Lexeme, size_t ByteCount) 
{
	if(BytesMatch(Lexeme, ByteCount))
	{
		RemoveWS();

		if(Extract(SelectedByteMirror) == '=')
		{
			SelectedByteMirror++;

			RemoveWS();

			if(TryToParseAttributeValue())
			{

				PopAttributeValueStack();
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

void xml_reader::PushNewMarkup(const char* StartTag, const char* EndTag)
{
	PushMarkupNode(MarkupNodeStack, xml_markup::Create(StartTag, EndTag));
}

void xml_reader::PopNewMarkup()
{
	xml_markup* MarkupNode = MarkupNodeStack.back();
	MarkupNodeStack.pop_back();

	assert(MarkupNode);
}

bool xml_reader::TryToParseDocumentDeclarationMarkup()
{
	const string StartTag = "<?xml";
	const string EndTag = "?>";

	if(!BytesMatch(StartTag.c_str(), StartTag.size()))
	{
		// lexing error encountered
		// report error & return;
		return false;
	}

	PushNewMarkup(StartTag.c_str(), EndTag.c_str());

	// expect ws following
	// opening tag
	if(!IsWS())
	{
		// lexing error encountered
		// report error & return;
		return false;
	}

	RemoveWS();

	const size_t AttributeCount = 3;
	const string Attributes[]
	{
		"version",
		"encoding",
		"standalone"
	};

	bool ParseFailed = false;

	for(size_t Index = 0; Index < AttributeCount; Index++)
	{
		if(!TryToParseAttribute(Attributes[Index].c_str(), 
								Attributes[Index].size()))
		{
			// error
			ParseFailed = true;
			break;
		}

		// check for required space
		if(!IsWS() && Attributes[Index] != "standalone")
		{
			// error
			ParseFailed = true;
			break;
		}
		
		PopAttributeValueStack();

		RemoveWS();
	}

	if(!ParseFailed)
	{
		if(!BytesMatch(EndTag.c_str(), EndTag.size()))
		{
			// log & report 
			// error
			return false;
		}

		PopNewMarkup();

		RemoveWS();

		return true;
	}

	return false;
}

void xml_reader::PopAttributeValueStack()
{
	if(!AttributeValueStack.empty())
	{
		xml_markup_attribute* Attribute = AttributeValueStack.back();
		AttributeValueStack.pop_back();

		// Attribute
	}
}

void xml_reader::PopMarkupNodeStack()
{
	if(!MarkupNodeStack.empty())
	{
		MarkupNodeStack.pop_back();
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
		if(!Doc->ParsedDeclaration())
		{
			if(TryToParseDocumentDeclarationMarkup())
			{
				// log & recorrd document 
				// parsing activity

				PopMarkupNodeStack();
			}
		}
	}

	return EndOfBuffer;
}
