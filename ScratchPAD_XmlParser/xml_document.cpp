#include "./include/xml_document.h"

xml_document::xml_document():
	FileHandle(nullptr),
	RootMarkupNode(nullptr)
{

}

xml_document::xml_document(const char* Path)
{
	if(!Open(Path))
	{
		// 
	}
}

xml_document::~xml_document()
{
	for(size_t Index = 0; Index < XMLDocumentAttributeEnumIndex(Count); Index++)
	{
		Attributes[Index].clear();
		Attributes[Index].shrink_to_fit();
	}

	if(FileHandle)
	{
		CloseFile(FileHandle);
		FileHandle = nullptr;
	}
}

bool xml_document::Open(const char* Path)
{
	bool OpenResult = false;

	FileHandle = OpenForReading(Path);

	if(FileHandle)
	{
		OpenResult = true;
	}

	return OpenResult;
}

bool xml_document::IsOpen() const
{
	return FileHandle->_IsOpen;
}

void xml_document::Close()
{
	CloseFile(FileHandle);
	FileHandle = nullptr;
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

bool xml_document::ParsedRequiredAttributes()
{
	return !Attributes[XMLDocumentAttributeEnumIndex(Version)].empty();
}
