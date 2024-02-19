#include "./include/xml_document.h"

namespace scratchpad
{
	xml_document::xml_document()
	{

	}

	xml_document::~xml_document()
	{
		for(size_t Index = 0; Index < (int) xml_decl_attributes::Count; Index++)
		{
			Decl[Index].clear();
			Decl[Index].shrink_to_fit();
		}
	}

	void xml_document::SetDocAttribute(xml_decl_attributes AttributeEnum, string Value)
	{
		if(AttributeEnum == xml_decl_attributes::Version)
		{
			Decl[(int) xml_decl_attributes::Version] = Value;
		}
		else if(AttributeEnum == xml_decl_attributes::Encoding)
		{
			Decl[(int) xml_decl_attributes::Encoding] = Value;
		}
		else if(AttributeEnum == xml_decl_attributes::Standalone)
		{
			Decl[(int) xml_decl_attributes::Standalone] = Value;
		}
	}

	void xml_document::PushMarkup(string&& Text)
	{
		move(Text);

	}

	void xml_document::PopMarkup()
	{

	}
}
