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

struct xml_document
{
	file*		FileHandle;
	string		Attributes[XMLDocumentAttributeEnumIndex(Count)];
	xml_markup* RootMarkupNode;

    xml_document(const char* Path);

	xml_document();
	~xml_document();

	bool Open(const char* Path);
	bool IsOpen() const;
	void Close();

	void SetDocAttribute(xml_builtin_doc_attributes AttributeEnum, string Value);
	bool ParsedDeclarationAttribute(xml_builtin_doc_attributes Attribute);
	bool ParsedDeclaration();

	static xml_document* CreateAndOpen(const char* Path);
	static xml_document* Create(const char* Path);

private:
	xml_document(const xml_document& Rhs) = delete;
	xml_document(xml_document&& Rhs) = delete;
};

#define XMLDocumentBuffer(XMLDoc)(XMLDoc->FileHandle->_Buffer)

#endif