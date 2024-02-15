#ifndef XML_DOCUMENT_H
#define XML_DOCUMENT_H

#include "win32_io.h"
#include "xml_markup.h"

enum class xml_builtin_doc_attributes
{
	Version    = 0,
	Encoding   = 1,
	Standalone = 2,
	Count      = 3
};

#define XMLDocumentAttributeEnum(Attribute)(xml_builtin_doc_attributes::Attribute)
#define XMLDocumentAttributeEnumIndex(Attribute)((int)xml_builtin_doc_attributes::Attribute)
#define XMLDocumentAttributeEnumFromIndex(Index)((xml_builtin_doc_attributes)Index)

class xml_document
{

public:
	xml_document(const char* Path);

	xml_document();
	~xml_document();

	bool Open(const char* Path);
	bool IsOpen() const;
	void Close();

	size_t GetFileSize();

	const char* BeginText();
	const char* EndText();

	void SetDocAttribute(xml_builtin_doc_attributes AttributeEnum, string Value);
	bool ParsedDeclarationAttribute(xml_builtin_doc_attributes Attribute);
	bool ParsedDeclaration();

	static xml_document* CreateAndOpen(const char* Path);
	static xml_document* Create(const char* Path);

private:
	string		Attributes[XMLDocumentAttributeEnumIndex(Count)];
	xml_markup* Markup;

public:
	bool  ParsedDecl;
	file* File;

private:
	xml_document(const xml_document& Rhs) = delete;
	xml_document(xml_document&& Rhs) = delete;

};

#endif