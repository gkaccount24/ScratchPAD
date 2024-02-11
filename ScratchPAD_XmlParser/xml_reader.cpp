#include "./include/xml_reader.h"

/**
*** INTERNAL HELPER MACROS
*/

#define Extract(SelectedByte)(*(SelectedByte))
#define AtEndOfBuffer(CurSel, BufferEnd)((CurSel) == (BufferEnd))

#define CurrentMarkupNode(NodeStack)(NodeStack.back())
#define PushMarkupNode(NodeStack, Node)(NodeStack.push_back(Node))
#define PushMarkupAttribute(AttributeStack, Attribute)(AttributeStack.push_back(Attribute))
#define AppendAttributeString(AttributeStack, AttributeValue, Length)\
	(AttributeStack.back()->Value\
				   .insert(0, AttributeValue, Length))\
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

		// MarkupNodeStack.resize(128);
		// MarkupAttributeStack.resize(128);

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

	MarkupAttributeStack.clear();
	MarkupAttributeStack.shrink_to_fit();

	MarkupNodeStack.clear();
	MarkupNodeStack.shrink_to_fit();
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
	WSSkipped = 0;

	while(SelectedByteMirror != BufferEnd && IsWS())
	{
		WSSkipped++;
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
	assert(!MarkupAttributeStack.empty());

	bool Parsed = false;

	size_t TempByteCount = 0;

	size_t StringByteCount = 0;
	const char* StringText = 0;

	if(Extract(SelectedByteMirror) == '\"')
	{		
		SelectedByteMirror++;
		TempByteCount++;

		StringText = SelectedByteMirror;
		
		while(SelectedByteMirror != BufferEnd)
		{
			if(Extract(SelectedByteMirror) == '\"')
			{
				SelectedByteMirror++;
				TempByteCount++;

				Parsed = true;
				break;
			}

			SelectedByteMirror++;
			TempByteCount++;
			StringByteCount++;
		}

		if(Parsed)
		{
			AppendAttributeString(MarkupAttributeStack, StringText, StringByteCount);

			SelectedByte = SelectedByteMirror;
			BufferPos += TempByteCount;
			BytesAvailable -= TempByteCount;
		}
	}

	return Parsed;
}

bool xml_reader::TryToParseAttribute(const char* AttributeName, size_t ByteCount) 
{
	if(!BytesMatch(AttributeName, ByteCount))
	{
		// lexing error encountered
		// report error & return;
		return false;
	}

	PushNewMarkupAttribute(AttributeName);

	RemoveWS();

	if(Extract(SelectedByteMirror) != '=')
	{
		// lexing error encountered
		// report error & return;
		return false;
	}
	
	SelectedByteMirror++;

	RemoveWS();

	if(!TryToParseAttributeValue())
	{
		return false;
	}

	return true;
}

void xml_reader::PushNewMarkupAttribute(const char* AttributeName)
{
	PushMarkupAttribute(MarkupAttributeStack, CreateAttribute(AttributeName));
}

void xml_reader::PushNewMarkup(const char* StartTag, const char* EndTag)
{
	PushMarkupNode(MarkupNodeStack, xml_markup::Create(StartTag, EndTag));
}

void xml_reader::PushNewMarkup(const char* NameToken, size_t Length)
{
	xml_markup* MarkupNode = nullptr;

	if(!MarkupNodeStack.empty())
	{
		if(MarkupNode = xml_markup::Create(NameToken, Length))
		{
			MarkupNodeStack.back()->Children.push_back(MarkupNode);
			PushMarkupNode(MarkupNodeStack, MarkupNode);
		}
	}
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

		RemoveWS();

		return true;
	}

	return false;
}

void xml_reader::PopMarkupNodeStack()
{
	if(MarkupNodeStack.empty())
	{
		// log occurrence & return
		return;
	}

	if(MarkupAttributeStack.empty())
	{
		// log occurrence & return
		return;
	}

	// reverse the array
	reverse(begin(MarkupAttributeStack), end(MarkupAttributeStack));

	const size_t AttributeCount = MarkupAttributeStack.size();

	for(size_t AttributeIndex = 0; AttributeIndex < AttributeCount; AttributeIndex++)
	{
		xml_markup_attribute* Attribute = MarkupAttributeStack.back();
		MarkupAttributeStack.pop_back();

		CurrentMarkupNode(MarkupNodeStack)->Attributes.push_back(Attribute);
	}


	xml_markup* MarkupNode = MarkupNodeStack.back();
	MarkupNodeStack.pop_back();

	if(!MarkupNodeStack.empty())
	{
		MarkupNodeStack.back()->Children.push_back(MarkupNode);
	}
	else
	{
		Doc->RootMarkupNode = MarkupNode;
	}
}

bool IsDelimiter(const char* SelectedByteMirror)
{
	bool Delimiter = (Extract(SelectedByteMirror) == ' ' || 
					  Extract(SelectedByteMirror) == '=' ||
					  Extract(SelectedByteMirror) == '\t' ||
					  Extract(SelectedByteMirror) == '\r' ||
					  Extract(SelectedByteMirror) == '\n' ||
					  Extract(SelectedByteMirror) == '>');

	return Delimiter;
}

bool xml_reader::TryToParseNameToken(char Delimiter, bool EatInitialByte)
{
	RemoveWS();

	if(EatInitialByte)
	{
		SelectedByte++;
		SelectedByteMirror++;

		BufferPos++;
		BytesAvailable--;
	}

	size_t NameTokenByteCount = 0;
	size_t NameTokenTotalByteCount = 0;

	const char* NameTokenText = SelectedByteMirror;

	bool ParsedToDelimiter = false;
	bool ParsedToWS = false;

	while(SelectedByteMirror != BufferEnd)
	{
		if(Extract(SelectedByteMirror) == Delimiter)
		{
			NameTokenTotalByteCount++;
			SelectedByteMirror++;
			ParsedToDelimiter = true;

			break;
		}
		else if(NameTokenByteCount > 0 && IsWS())
		{
			ParsedToWS = true;
			break;
		}
		else
		{
			SelectedByteMirror++;
			NameTokenByteCount++;
			NameTokenTotalByteCount++;
		}
	}

	if(ParsedToDelimiter || ParsedToWS)
	{
		NameTokenStack.push_back(CreateNameToken(NameTokenText, NameTokenByteCount));

		SelectedByte = SelectedByteMirror;
		BufferPos += NameTokenTotalByteCount;
		BytesAvailable -= NameTokenTotalByteCount;

		return ParsedToDelimiter;
	}

	return ParsedToDelimiter;
}

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

				// PopMarkupNodeStack();
			}
		}

		bool Error = false;

		while(SelectedByte != BufferEnd && !Error)
		{
			switch(Extract(SelectedByte))
			{
				case '<':
				{
					if(!TryToParseNameToken('>'))
					{ 
						while(TryToParseNameToken('=', false))
						{
							PushNewMarkupAttribute(NameTokenStack.back()->Name.c_str());
						}
					}
					else
					{
						// get last name token
						if(!NameTokenStack.empty())
						{
							PushNewMarkup(NameTokenStack.back()->Name.c_str(),
										  NameTokenStack.back()->Name.size());
						}
					}

					RemoveWS();
					
					break;
				}
				case '>':
				{
					if(!false)
					{
						// log and report error
						Error = true;
					}

					break;
				}
			}
		}
	}

	return EndOfBuffer;
}
