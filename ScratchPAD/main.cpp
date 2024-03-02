#include "source.h"
#include "parser.h"
#include "document.h"

#include <string_view>
#include <initializer_list>

using std::initializer_list;
using std::string_view;
using std::pair;

#undef main

static constexpr size_t XMLSourceFileCount = 9;

static constexpr string_view XMLSourceFiles[XMLSourceFileCount]
{
	"Z:\\data\\xml samples\\books.xml",
	"Z:\\data\\xml samples\\supplier.xml",
	"Z:\\data\\xml samples\\region.xml",
	"Z:\\data\\xml samples\\partsupp_pv.xml",
	"Z:\\data\\xml samples\\part_pv.xml",
	"Z:\\data\\xml samples\\orders_pv.xml",
	"Z:\\data\\xml samples\\nation.xml",
	"Z:\\data\\xml samples\\lineitem_pv.xml",
	"Z:\\data\\xml samples\\customer_pv.xml"
};

class file_switcher
{
	struct file_idx
	{
		string				     Path;
		scratchpad::xml::source* Source;
	};

public:

	 file_switcher(const string_view* SourceFilePaths, size_t SourceFileCount);
	~file_switcher();

	int32_t OpenFile(string FilePath);
	void CloseFile(size_t FileIdx);

private:

	vector<file_idx> Files;

};

file_switcher::file_switcher(const string_view* SourceFiles, size_t SourceFileCount)
{
	/***
	**** FILE SWITCHER 
	***** CONSTRUCTOR
	***/

	for(size_t Idx = 0; Idx < SourceFileCount; Idx++)
	{
		if(!SourceFiles[Idx].empty())
		{
			file_idx NewFileIdx 
			{ 
				/**
				*** PATH AS C 
				*** STYLE STRING
				**/
				SourceFiles[Idx].data(),

				/**
				*** DEFAULT
				*** SOURCE PTR
				**/
				nullptr
			};

			Files.push_back(NewFileIdx);
		}
	}
}

file_switcher::~file_switcher()
{
	/***
	**** FILE SWITCHER 
	***** DESTRUCTOR 
	***/

	size_t FileCount = Files.size();

	for(size_t Idx = 0; Idx < FileCount; Idx++)
	{
		if(Files[Idx].Source)
		{
			Files[Idx].Source->Close();

			delete Files[Idx].Source;
			Files[Idx].Source = nullptr;
		}
	}
}

int32_t file_switcher::OpenFile(string FilePath)
{
	if(FilePath.empty())
	{
		file_idx NewFileIdx 
		{ 
			/**
			*** PATH AS C 
			*** STYLE STRING
			**/
			FilePath,

			/**
			*** DEFAULT
			*** SOURCE PTR
			**/
			new scratchpad::xml::source(FilePath)
		};

		if(NewFileIdx.Source->File.is_open())
		{
			Files.push_back(NewFileIdx);

			return Files.size() - 1;
		}
	}

	return -1;
}

void file_switcher::CloseFile(size_t FileIdx)
{
	if(!Files.empty())
	{
		size_t FileCount = Files.size();

		if(FileIdx < FileCount && FileIdx > 0)
		{
			Files[FileIdx].Source->Close();

			delete Files[FileIdx].Source;
			Files[FileIdx].Source = nullptr;
		}
	}
}

int main(int ArgCount, char* ArgV[])
{
	file_switcher FileSwitcher(XMLSourceFiles, XMLSourceFileCount);

	/***
	**** PRIMARY PARSED XML 
	**** DOCUMENT
	***/
	scratchpad::xml::document* XMLDocument = nullptr;

	/***
	**** PRIMARY SOURCE &
	**** PARSER code instances
	***/
	scratchpad::xml::source XMLSource;
	scratchpad::xml::parser XMLParser;

	return XMLParser.Diagnostics->ExitCode;
}