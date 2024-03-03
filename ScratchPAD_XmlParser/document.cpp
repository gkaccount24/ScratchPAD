#include "document.h"

namespace scratchpad
{
	namespace xml
	{
		void document::Cleanup(markup* Markup)
		{
			const size_t AttributeCount = Markup->Attributes.size();
			const size_t ChildCount = Markup->Children.size();

			for(size_t Idx = 0; Idx < AttributeCount; Idx++)
			{
				delete Markup->Attributes[Idx];

				Markup->Attributes[Idx] = nullptr;
			}

			for(size_t Idx = 0; Idx < ChildCount; Idx++)
			{
				Cleanup(Markup->Children[Idx]);

				Markup->Children[Idx] = nullptr;
			}

			Markup->Content.clear();
			Markup->StartTag.clear();
			Markup->EndTag.clear();

			delete Markup;
			Markup = nullptr;
		}

		void document::Close()
		{
			if(RootMarkup)
			{
				Cleanup(RootMarkup);

				RootMarkup = nullptr;
			}

			Decl[(int)xml_decl_attributes::Version].clear();
			Decl[(int)xml_decl_attributes::Encoding].clear();
			Decl[(int)xml_decl_attributes::Standalone].clear();

			ParsedDecl = false;
		}

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
