#ifndef XML_READER_H
#define XML_READER_H

#include <win32_io.h>

#include <iostream>
#include <string>

using std::cout;
using std::flush;
using std::endl;

using std::string;

#define XmlDocBuf(Doc)((Doc)->GetBufferRef())

#define Extract(SelectedByte)(*(SelectedByte))
#define AtEndOfBuffer(CurSel, BufferEnd)((CurSel) == (BufferEnd))

class xml_doc 
{

public:
	 xml_doc() = default;
	~xml_doc() = default;

public:
	bool Open(const char* Path);
	bool IsOpen() const;
	void Close();

public:
	inline string& GetBufferRef() const { return FileHandle->_Buffer; }

public:
	static xml_doc* CreateAndOpen(const char* Path);
	static xml_doc* Create(const char* Path);

private:
	file* FileHandle;

	// xml doc state members
	uint16_t Version;
	string VersionString;
	bool ParsedTypeAndDecl;

};

#define MarkupTagTypeIndex(MarkupTag) (static_cast<int>((MarkupTag)))

enum class xml_markup_tag_type
{
	Start = 0,
	End = 1
};

#define MAX_XML_TAG_ATTRIBUTE_COUNT 128
#define BUILTIN_MARKUP_TAG_COUNT 5

#define BuiltinMarkupTagTypeEnum(MarkupTag)(xml_builtin_markup_tags::MarkupTag)
#define BuiltinMarkupTagIndex(MarkupTag)(static_cast<int>(MarkupTag))
#define BuiltinMarkupTag(MarkupTag)(xml_reader::GetBuiltinMarkupTag(BuiltinMarkupTagTypeEnum(MarkupTag)))

enum class xml_builtin_markup_tag_attributes
{
	XMLVersion = 0,
	DocEncoding	= 1,
	StandaloneDecl = 2
};

#define BuiltinMarkupTagAttributeEnum(Attribute)(xml_builtin_markup_tag_attributes::Attribute)
#define BuiltinMarkupTagAttributeIndex(Attribute) static_cast<int>(BuiltinMarkupTagAttributeEnum(Attribute))
#define BuiltinMarkupTagAttribute(MarkupTag, Attribute)(BuiltinMarkupTag(MarkupTag)->Attributes[BuiltinMarkupTagAttributeIndex(Attribute)])

struct xml_builtin_markup_tag_attribute
{
	xml_builtin_markup_tag_attributes AttributeName;
	const char* ExpectedBytes;
	size_t ByteCount;
};

enum class xml_builtin_markup_tags
{
	CommentTag = 0,
	CharDataTag = 1,
	ProcessingInstructionTag = 2,
	PrologAndTypeDeclTag = 3,
	CDataSectionTag = 4
};

struct xml_builtin_markup_tag 
{
	xml_builtin_markup_tags TagType;

	const char* StartText;
	size_t StartTextLength;

	const char* EndText;
	size_t EndTextLength;

	xml_builtin_markup_tag_attribute* Attributes;
	size_t AttributeCount;
};

#define XMLReaderMode(ReaderModeEnum)(xml_reader_mode::ReaderModeEnum)
#define XMLReaderModeIndex(ReaderModeEnum)(static_cast<int>(XMLReaderMode(ReaderModeEnum)))

enum class xml_reader_mode
{
	ReadingCommentTag = 0,
	ReadingCharDataTag = 1,
	ReadingProcessingInstructionTag = 2,
	ReadingPrologAndTypeDeclTag = 3,
	ReadingCDataSectionTag = 4
};

class xml_reader
{

public:
	 xml_reader() = default;
	~xml_reader() = default;

public:
	bool Read(xml_doc* Doc);

private:
	void CopyByteRange(const char* SrcBytes, char* DstBytes, size_t SrcByteCount);
	bool CompareBytes(const char* SelectedByteCopy, const char* SrcBytes, size_t ByteCount);

	bool IsCommentTag();
	bool IsCharDataTag();
	bool IsProcessingInstructionTag();
	bool IsPrologAndTypeDeclTag();
	bool IsCDataSectionTag();

	void SkipWS();
	void Advance(size_t ByteCount = 1);
	bool IsWhitespace() const;

private:
	bool TryToParseVersionAttribute();
	bool TryToParseDocEncodingAttribute();
	bool TryToParseStandaloneDeclAttribute();
	bool TryToParseAttributeEquals();
	bool TryToParseAttributeValue();
	bool TryToParseQuote();

public:
	inline static const xml_builtin_markup_tag* GetBuiltinMarkupTag(xml_builtin_markup_tags Tag)
	{
		static xml_builtin_markup_tag_attribute XML_DECL_TAG_BUILTIN_ATTRIBUTES[]
		{
			xml_builtin_markup_tag_attribute  { BuiltinMarkupTagAttributeEnum(XMLVersion), "version", 7 },
			xml_builtin_markup_tag_attribute  { BuiltinMarkupTagAttributeEnum(DocEncoding), "encoding", 8 },
			xml_builtin_markup_tag_attribute  { BuiltinMarkupTagAttributeEnum(StandaloneDecl), "standalone", 10 },
		};

		static xml_builtin_markup_tag BUILTIN_MARKUP_TAGS[BUILTIN_MARKUP_TAG_COUNT] 
		{
			xml_builtin_markup_tag { BuiltinMarkupTagTypeEnum(CommentTag), "<!--", 4, "-->", 3, 0, 0 },
			xml_builtin_markup_tag { BuiltinMarkupTagTypeEnum(CharDataTag), "<", 1, ">", 1, 0, 0 },
			xml_builtin_markup_tag { BuiltinMarkupTagTypeEnum(ProcessingInstructionTag), "<?", 2, "?>", 2, 0, 0 },
			xml_builtin_markup_tag { BuiltinMarkupTagTypeEnum(PrologAndTypeDeclTag), "<?xml", 5, "?>", 2, XML_DECL_TAG_BUILTIN_ATTRIBUTES, 3 },
			xml_builtin_markup_tag { BuiltinMarkupTagTypeEnum(CDataSectionTag), "<![DATA[", 9, "]]>", 4, 0, 0 },
		};

		return &BUILTIN_MARKUP_TAGS[BuiltinMarkupTagIndex(Tag)];
	}

private:

	// Current Reading Mode
	xml_reader_mode Mode;

	// Buffer Range
	const char* BufferBegin;
	const char* BufferEnd;

	// Selected Byte
	const char* SelectedByte;
	const char* SelectedByteMirror;

	// Position and Buffer size
	size_t BufferPos;
	size_t BufferSize;
	size_t BytesAvailable;

	// Reading State flags
	bool EndOfBuffer;

};

#endif
