#include "./include/xml_reader.h"

/**
*** INTERNAL HELPER MACROS
*/

#define Extract(SelectedByte)(*(SelectedByte))
#define AtEndOfBuffer(CurSel, BufferEnd)((CurSel) == (BufferEnd))

/**
*** XML_READER
*/

xml_reader::xml_reader(xml_document* XMLDoc):
	Doc(nullptr), BufferSize(0),
	BufferPos(0), BufferBegin(nullptr),
	BufferEnd(nullptr), BytesAvailable(0),
	SelectedByte(nullptr), SelectedByteMirror(nullptr),
	EndOfBuffer(false), WSSkipped(0)
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

	MarkupStack.clear();
	MarkupStack.shrink_to_fit();
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
		SelectedByteMirror = SelectedByte;

		return false;
	}
}

bool xml_reader::TryToParseAttributeValue()
{
	assert(!MarkupStack.empty());

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
			MarkupAttributeStack.back()->Value.insert(0, StringText, StringByteCount);

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

	PushMarkupAttribute(AttributeName, ByteCount);

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

void xml_reader::PushMarkupAttribute(const char* AttributeName, size_t ByteCount)
{
	MarkupAttributeStack.push_back(CreateAttribute(AttributeName));
}

void xml_reader::PushMarkup(const char* StartTag, const char* EndTag)
{
	MarkupStack.push_back(xml_markup::Create(StartTag, EndTag));
}

void xml_reader::PushMarkup(const char* NameToken, size_t Length)
{
	xml_markup* MarkupNode = nullptr;

	if(MarkupNode = xml_markup::Create(NameToken, Length))
	{
		if(!MarkupStack.empty())
		{
			MarkupStack.back()->Children.push_back(MarkupNode);
		}

		MarkupStack.push_back(MarkupNode);
	}
}

void xml_reader::PopMarkup()
{
	size_t NodeAttributeCount = MarkupStack.back()->Attributes.size();

	for(size_t I = 0; I < NodeAttributeCount; I++)
	{
		MarkupAttributeStack.pop_back();
	}

	MarkupStack.pop_back();
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

	PushMarkup(StartTag.c_str(), EndTag.c_str());

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

	Doc->ParsedDeclarationMarkup = true;

	for(size_t Index = 0; Index < AttributeCount; Index++)
	{
		if(!TryToParseAttribute(Attributes[Index].c_str(), 
								Attributes[Index].size()))
		{
			Doc->ParsedDeclarationMarkup = false;
			break;
		}

		// check for required space
		if(!IsWS() && Attributes[Index] != "standalone")
		{
			Doc->ParsedDeclarationMarkup = false;
			break;
		}

		MarkupStack.back()->Attributes.push_back(MarkupAttributeStack.back());
		
		RemoveWS();
	}

	if(Doc->ParsedDeclarationMarkup)
	{
		if(!BytesMatch(EndTag.c_str(), EndTag.size()))
		{
			Doc->ParsedDeclarationMarkup = false;

			return Doc->ParsedDeclarationMarkup;
		}

		RemoveWS();

		return Doc->ParsedDeclarationMarkup;
	}

	return Doc->ParsedDeclarationMarkup;
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
		NameTokenStack.push_back(xml_name_token::Create(NameTokenText, NameTokenByteCount));

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
			if(!TryToParseDocumentDeclarationMarkup())
			{
				// log and 
				// report error
				return false;
			}
		}

		bool Error = false;

		while(SelectedByte != BufferEnd && !Error)
		{
			if(Extract(SelectedByte) == '<')
			{
				bool MatchedEndTag = false;

				if(!MarkupStack.empty())
				{
					xml_markup* MarkupNode = MarkupStack.back();

					// empty character data buffer
					if(!CharDataBuf.empty())
					{
						xml_markup* MarkupNode = MarkupStack.back();

						MarkupNode->Text = move(CharDataBuf);

						CharDataBuf.clear();
					}

					if(BytesMatch(MarkupNode->EndTag.c_str(), 
								  MarkupNode->EndTag.size()))
					{
						MatchedEndTag = true;

						MarkupStack.pop_back();

						RemoveWS();
					}
				}

				if(!MatchedEndTag && !TryToParseNameToken('>'))
				{
					PushMarkup(NameTokenStack.back()->Name.c_str(),
							   NameTokenStack.back()->Name.size());

					while(Extract(SelectedByteMirror) != '>' && TryToParseNameToken('=', false))
					{
						// parsed attribute assignment
						PushMarkupAttribute(NameTokenStack.back()->Name.c_str(), 
											NameTokenStack.back()->Name.size());

						if(!TryToParseAttributeValue())
						{
							Error = true;

							break;
						}

						if(!IsWS() && Extract(SelectedByteMirror) != '>')
						{
							Error = true;

							break;
						}

						MarkupStack.back()->Attributes.push_back(MarkupAttributeStack.back());
					}

					if(Extract(SelectedByteMirror) != '>' && !Error)
					{
						Error = true;

						break;
					}

					SelectedByte++;
					SelectedByteMirror++;

					BufferPos++;
					BytesAvailable--;

					RemoveWS();
				}
				else if(!MatchedEndTag)
				{
					if(!NameTokenStack.empty())
					{
						PushMarkup(NameTokenStack.back()->Name.c_str(),
								   NameTokenStack.back()->Name.size());
					}

					RemoveWS();
				}
			}
			else if(Extract(SelectedByte) > 0 &&
					Extract(SelectedByte) < 128)
			{
				CharDataBuf.push_back(Extract(SelectedByte));

				SelectedByte++;
				SelectedByteMirror++;

				BufferPos++;
				BytesAvailable--;
			}
		}

		std::cout << "wait here";
		std::cin.get();
	}

	return EndOfBuffer;
}
