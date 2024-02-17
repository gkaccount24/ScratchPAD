#include "./include/xml_document.h"

xml_document::xml_document():
	Markup(nullptr),
	ParsedDecl(false),
	File(new file())
{
}

xml_document::xml_document(const char* Path):
	Markup(nullptr),
	ParsedDecl(false),
	File(new file())
{
	if(!Open(Path))
	{
		// log & report error
		// set bad state flags
	}
}

xml_document::~xml_document()
{
	for(size_t Index = 0; Index < XMLDocumentAttributeEnumIndex(Count); Index++)
	{
		Attributes[Index].clear();
		Attributes[Index].shrink_to_fit();
	}

	if(File)
	{
		File->Close();

		delete File;
		File = nullptr;
	}
}

bool xml_document::Open(const char* Path)
{
	if(File)
	{
		if(File->IsOpen)
		{
			return false;
		}

		return File->OpenForReading(Path);
	}

	// this shouldn't ever be the case
	// because our constructor always
	// allocates memory for a file obj
	return false;
}

bool xml_document::IsOpen() const
{
	return File->IsOpen;
}

void xml_document::Close()
{
	if(File)
	{
		File->Close();

		delete File;
		File = nullptr;
	}
}

xml_document* xml_document::CreateAndOpen(const char* Path)
{
	xml_document* Doc = new xml_document(Path);

	if(!Doc)
	{
		return nullptr;
	}

	if(!Doc->IsOpen())
	{
		delete Doc;
		Doc = nullptr;
	}

	return Doc;
}

xml_document* xml_document::Create(const char* Path)
{
	xml_document* Doc = new xml_document();

	if(!Doc)
	{
		return nullptr;
	}

	return Doc;
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

const char* xml_document::BeginText()
{
	return File->BeginBuffer();
}

const char* xml_document::EndText()
{
	return File->EndBuffer();
}

size_t xml_document::GetFileSize()
{
	return File->GetSizeOnDisk();
}

bool xml_document::ParsedDeclarationAttribute(xml_builtin_doc_attributes Attribute)
{
	return false;
}

bool xml_document::ParsedDeclaration()
{
	return ParsedDecl;
}
