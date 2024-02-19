#include "./include/xml_reader.h"

/**
*** INTERNAL HELPER MACROS
***/

#define Extract(SelectedByte)(*(SelectedByte))
#define AtEndOfBuffer(CurSel, BufferEnd)((CurSel) == (BufferEnd))

#if 0
namespace scratchpad
{
	/**
	*** XML_READER
	*/

	xml_reader::xml_reader(scratchpad::xml_document& XMLDoc,
						   scratchpad::logger& XMLReaderLogger):
		Doc(nullptr), BufferSize(0),
		BufferPos(0), BufferBegin(nullptr),
		BufferEnd(nullptr), BytesAvailable(0),
		SelectedByte(nullptr), SelectedByteMirror(nullptr),
		EndOfBuffer(false), WSSkipped(0),
		Logger(XMLReaderLogger)

	{
		if(XMLDoc && XMLDoc->IsOpen())
		{
			Doc = XMLDoc;

			BufferSize = XMLDoc->GetFileSize();
			BufferPos = 0;

			// Buffer Range
			BufferBegin = XMLDoc->BeginText();
			BufferEnd = XMLDoc->EndText();

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

		if(Extract(SelectedByteMirror) != '\"')
		{
			OutputError("expected '\"'\r\n");

			return false;
		}

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
			else if(Extract(SelectedByteMirror) == '<')
			{
				// illegal character encountered
				OutputError("illegal character encountered '<'\r\n");

				break;
			}

			SelectedByteMirror++;
			TempByteCount++;
			StringByteCount++;
		}

		if(!Parsed)
		{
			return false;
		}

		MarkupAttributeStack.back()->Value.insert(0, StringText, StringByteCount);

		SelectedByte = SelectedByteMirror;
		BufferPos += TempByteCount;
		BytesAvailable -= TempByteCount;

		return Parsed;
	}

	bool xml_reader::TryToParseAttribute(const char* AttributeName, size_t ByteCount)
	{
		if(!BytesMatch(AttributeName, ByteCount))
		{
			OutputError("failed to match attribute name\r\n");

			return false;
		}

		PushMarkupAttribute(AttributeName, ByteCount);

		RemoveWS();

		if(Extract(SelectedByteMirror) != '=')
		{
			OutputError("expected '='\r\n");

			return false;
		}

		SelectedByteMirror++;

		RemoveWS();

		if(!TryToParseAttributeValue())
		{
			OutputError("failed to parse attribute value\r\n");

			return false;
		}

		return true;
	}

	void xml_reader::PushMarkupAttribute(const char* AttributeName, size_t ByteCount)
	{
		if(xml_markup_attribute* Attribute = xml_markup_attribute::Create(AttributeName))
		{
			MarkupAttributeStack.push_back(Attribute);
		}
	}

	void xml_reader::PushMarkup(const char* StartTag, const char* EndTag)
	{
		if(xml_markup* Markup = xml_markup::Create(StartTag, EndTag))
		{
			MarkupStack.push_back(Markup);
		}
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
			OutputNotice("no xml declaration found.\r\n");

			return false;
		}

		PushMarkup(StartTag.c_str(), EndTag.c_str());

		// expect ws following
		// opening tag
		if(!IsWS())
		{
			OutputError("expected ' '");

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

		bool ParsedDeclMarkup = true;

		for(size_t Index = 0; Index < AttributeCount; Index++)
		{
			if(!TryToParseAttribute(Attributes[Index].c_str(),
			   Attributes[Index].size()))
			{
				OutputError("failed to parse xml attribute\r\n");

				ParsedDeclMarkup = false;
				break;
			}

			// check for required space
			if(!IsWS() && Attributes[Index] != "standalone")
			{
				OutputError("expected ' '\r\n");

				ParsedDeclMarkup = false;
				break;
			}

			MarkupStack.back()->Attributes.push_back(MarkupAttributeStack.back());

			RemoveWS();
		}

		if(ParsedDeclMarkup)
		{
			if(!BytesMatch(EndTag.c_str(), EndTag.size()))
			{
				OutputError("failed to match xml declaration end tag\r\n");

				Doc->ParsedDecl = false;

				return Doc->ParsedDecl;
			}

			RemoveWS();

			Doc->ParsedDecl = true;

			return Doc->ParsedDecl;
		}


		OutputError("failed to parse xml declaration\r\n");

		Doc->ParsedDecl = false;

		return false;
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
		if(Doc && !Doc->IsOpen())
		{
			OutputError("xml document file is not open for reading. \r\n");

			return false;
		}

		if(!Doc->ParsedDeclaration())
		{
			if(!TryToParseDocumentDeclarationMarkup())
			{
				OutputNotice("failed to parse xml declaration\r\n");
			}
			else
			{
				OutputNotice("successfully parsed xml declaration\r\n");
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

							OutputError("failed to parse xml attribute\r\n");

							break;
						}

						if(!IsWS() && Extract(SelectedByteMirror) != '>')
						{
							Error = true;

							OutputError("expected '>' or ' '\r\n");

							break;
						}

						MarkupStack.back()->Attributes.push_back(MarkupAttributeStack.back());
					}

					if(Extract(SelectedByteMirror) != '>' || !Error)
					{
						Error = true;

						OutputError("expected '>'\r\n");

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
					if(NameTokenStack.empty())
					{
						// ERROR HANDLE IT ???
						break;
					}

					PushMarkup(NameTokenStack.back()->Name.c_str(),
							   NameTokenStack.back()->Name.size());
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

		return EndOfBuffer;
	}

	void xml_reader::AttachLogger(logger* XMLReaderLogger)
	{
		if(!Logger)
		{
			// log failure to set logger;
		}

		Logger = XMLReaderLogger;
	}

	void xml_reader::OutputInfo(const char* Message)
	{
		if(Logger && LoggingEnabled)
		{
			Logger->ConsoleInfo(Message);
		}
	}

	void xml_reader::OutputNotice(const char* Message)
	{
		if(Logger && LoggingEnabled)
		{
			Logger->ConsoleNotice(Message);
		}
	}

	void xml_reader::OutputWarning(const char* Message)
	{
		if(Logger && LoggingEnabled)
		{
			Logger->ConsoleWarning(Message);
		}
	}

	void xml_reader::OutputError(const char* Message)
	{
		if(Logger && LoggingEnabled)
		{
			Logger->ConsoleError(Message);
		}
	}
}
#endif