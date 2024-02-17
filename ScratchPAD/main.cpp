#include "xml_document.h"
#include "xml_reader.h"

#include <iostream>

using std::cout;
using std::endl;

//while(!EndOfBuffer)
//{
//	SkipWS();
//
//	switch(Extract(SelectedByte))
//	{
//		case '<':
//		{
//			const xml_markup* Markup = GetBuiltinMarkupTags();
//
//			for(size_t Index = 0; Index < BUILTIN_MARKUP_TAG_COUNT; Index++)
//			{
//				if(Markup[Index].TagType == BuiltinMarkupTagTypeEnum(PrologAndTypeDeclTag) && Doc->ParsedVersion())
//				{
//					continue;
//				}
//
//				if(CompareBytes(Markup[Index].StartText,
//								Markup[Index].StartTextLength))
//				{
//					Advance(Markup[Index].StartTextLength);
//					SkipWS();
//
//					for(size_t JIndex = 0; JIndex < Markup[Index].AttributeCount; JIndex++)
//					{
//						xml_markup_attribute Attribute = Markup[Index].Attributes[JIndex];
//
//						if(CompareBytes(Attribute.ExpectedBytes, Attribute.ByteCount))
//						{
//							Advance(Attribute.ByteCount);
//							SkipWS();
//
//							if(CompareBytes("=", 1))
//							{
//								Advance();
//								SkipWS();
//
//								if(TryToParseAttributeValue())
//								{
//									if(Markup[Index].TagType == xml_builtin_markup_tags::PrologAndTypeDeclTag)
//									{
//										Doc->SetDocAttribute(XMLDocumentAttributeEnumFromIndex(JIndex), AttributeValueStack[AttributeValueCount - 1]);
//
//										Advance(Doc->Attributes[JIndex].size() + 2);
//									}
//
//									PopAttributeValueStack();
//								}
//								else
//								{
//									// break;
//									break;
//								}
//							}
//							else
//							{
//								// Success = false;
//								break;
//							}
//						}
//
//						SkipWS();
//					}
//
//					if(CompareBytes(Markup[Index].EndText,
//									Markup[Index].EndTextLength))
//					{
//						Advance(Markup[Index].EndTextLength);
//						SkipWS();
//					}
//					else
//					{
//						// SelectedByte = SelectedByteMirror;
//					}
//
//					break;
//				}
//				else
//				{
//					//
//
//				}
//			}
//
//			break;
//		}
//	}
//
//	EndOfBuffer = SelectedByte == BufferEnd;
//}

enum exit_code
{
	Success			= 0,

	// ERRORS
	FailedToOpenDoc = -100,
	FailedToReadDoc = -101
};

#define SAMPLE_FILE_COUNT 6

int main(int ArgCount, char* ArgV[])
{
	exit_code ExitCode = Success;

	logger* Logger = new logger();

	xml_document* XMLDoc = nullptr;

	XMLDoc = xml_document::CreateAndOpen("Z:\\data\\xml samples\\books.xml");

	if(!XMLDoc)
	{
		ExitCode = FailedToOpenDoc;
	}
	else
	{
		xml_reader Reader(XMLDoc);

		Reader.AttachLogger(Logger);

		if(!Reader.Read())
		{
			ExitCode = FailedToReadDoc;
		}
		else
		{
			// do stuff with the xml doc

		}
	}

	if(Logger)
	{
		delete Logger;
		Logger = nullptr;
	}

	return ExitCode;
}