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

private:
	xml_doc(const xml_doc& Rhs) = delete;
	xml_doc(xml_doc&& Rhs) = delete;

public:
	bool Open(const char* Path);
	bool IsOpen() const;
	void Close();

public:
	inline string& GetBufferRef() const { return FileHandle->_Buffer; }

	inline void SetVersion(string DocVersion) { Version = DocVersion; }
	inline string GetVersion() const { return Version; }

	inline void SetEncoding(string DocEncoding) { Encoding = DocEncoding; }
	inline string GetEncoding() const { return Encoding; }

	inline void SetStandaloneDecl(string DocStandaloneDecl) { StandaloneDecl = DocStandaloneDecl; }
	inline string GetStandalone() const { return StandaloneDecl; }

public:
	static xml_doc* CreateAndOpen(const char* Path);
	static xml_doc* Create(const char* Path);

private:
	file* FileHandle;

	// xml doc state members
	string Version;
	string Encoding;
	string StandaloneDecl;

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
	string Value;
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

private:
	xml_reader(const xml_reader& Rhs) = delete;
	xml_reader(xml_reader&& Rhs) = delete;

public:
	bool Read(xml_doc* Doc);

private:
	bool CompareBytes(const char* SrcBytes, size_t ByteCount, bool RewindMirror = false);

	bool IsCommentTag(bool RewindMirror = false);
	bool IsCharDataTag(bool RewindMirror = false);
	bool IsProcessingInstructionTag(bool RewindMirror = false);
	bool IsPrologAndTypeDeclTag(bool RewindMirror = false);
	bool IsCDataSectionTag(bool RewindMirror = false);

	void SkipWS();

	void Advance(size_t ByteCount = 1, bool LookAhead = false);
	void LookAhead(size_t ByteCount = 1, bool RewindMirror = false);
	void Rewind(size_t ByteCount = 1);
	bool IsWhitespace() const;

	void PopAttributeValueStack();

private:
	bool TryToParseVersionAttribute(bool RewindMirror = false);
	bool TryToParseDocEncodingAttribute(bool RewindMirror = false);
	bool TryToParseStandaloneDeclAttribute(bool RewindMirror = false);
	bool TryToParseAttributeEquals(bool RewindMirror = false);
	bool TryToParseAttributeValue(bool RewindMirror = false);
	bool TryToParseQuote(bool RewindMirror = false);

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
	// the current document were 
	// suppose to be reading
	xml_doc* Doc;

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

	inline static constexpr int ATTRIBUTE_VALUE_STACK_COUNT = 64;

	string AttributeValueStack[ATTRIBUTE_VALUE_STACK_COUNT];
	size_t CurrentAttributeValueCount;

};

#endif