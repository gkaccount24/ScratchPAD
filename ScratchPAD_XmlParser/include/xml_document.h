#ifndef XML_DOCUMENT_H
#define XML_DOCUMENT_H

#include "xml_source.h"
#include "xml_markup.h"

#define XMLDocumentAttributeEnum(Attribute)(xml_builtin_doc_attributes::Attribute)
#define XMLDocumentAttributeEnumIndex(Attribute)((int)xml_builtin_doc_attributes::Attribute)
#define XMLDocumentAttributeEnumFromIndex(Index)((xml_builtin_doc_attributes)Index)

namespace scratchpad
{
	enum class xml_builtin_doc_attributes
	{
		Version = 0,
		Encoding = 1,
		Standalone = 2,
		Count = 3
	};

	class xml_document
	{

	public:
		xml_document(xml_source&& Source);

		// default constructors
		xml_document();
		~xml_document();

	public:
		void SetDocAttribute(xml_builtin_doc_attributes AttributeEnum, string Value);
		bool ParsedDeclaration();

	private:
		xml_markup* Markup;
		string		Attributes[XMLDocumentAttributeEnumIndex(Count)];

	public:
		bool		ParsedDecl;

	private:
		xml_document(const xml_document& Rhs) = delete;
		xml_document(xml_document&& Rhs) = delete;

	};
}

#endif