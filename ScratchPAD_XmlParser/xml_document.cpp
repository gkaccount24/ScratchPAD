#include "./include/xml_document.h"

namespace scratchpad
{
	xml_document::xml_document(xml_source&& Source):
		Markup(nullptr),
		ParsedDecl(false)
	{

	}

	xml_document::xml_document():
		Markup(nullptr),
		ParsedDecl(false)
	{
	}

	xml_document::~xml_document()
	{
		for(size_t Index = 0; Index < XMLDocumentAttributeEnumIndex(Count); Index++)
		{
			Attributes[Index].clear();
			Attributes[Index].shrink_to_fit();
		}
	}

	void xml_document::SetDocAttribute(xml_builtin_doc_attributes AttributeEnum, string Value)
	{
		if(AttributeEnum == XMLDocumentAttributeEnum(Version))
		{
			Attributes[XMLDocumentAttributeEnumIndex(Version)] = Value;
		}
		else if(AttributeEnum == XMLDocumentAttributeEnum(Encoding))
		{
			Attributes[XMLDocumentAttributeEnumIndex(Encoding)] = Value;
		}
		else if(AttributeEnum == XMLDocumentAttributeEnum(Standalone))
		{
			Attributes[XMLDocumentAttributeEnumIndex(Standalone)] = Value;
		}
	}

	bool xml_document::ParsedDeclaration()
	{
		return ParsedDecl;
	}
}
