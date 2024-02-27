#include "document.h"

namespace scratchpad
{
	namespace xml
	{
		void document::SetDocAttribute(xml_decl_attributes AttributeEnum, string Value)
		{
			if(AttributeEnum == xml_decl_attributes::Version)
			{
				Decl[(int)xml_decl_attributes::Version] = Value;
			}
			else if(AttributeEnum == xml_decl_attributes::Encoding)
			{
				Decl[(int)xml_decl_attributes::Encoding] = Value;
			}
			else if(AttributeEnum == xml_decl_attributes::Standalone)
			{
				Decl[(int)xml_decl_attributes::Standalone] = Value;
			}
		}
	}
}
