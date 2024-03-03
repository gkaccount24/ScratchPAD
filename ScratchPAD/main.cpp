#include "source.h"
#include "parser.h"
#include "document.h"

#include <string_view>

using std::string_view;

#undef main

namespace scratchpad
{
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
		/*** file index
		**** type for holding
		**** switching xml files
		***/
		struct file_idx
		{
			string				       Path;
			scratchpad::xml::source* Source;
			scratchpad::xml::document* Document;
			bool Parsed;
		};

	public:

		file_switcher();
		~file_switcher();

	public:

		/*** methods for managing
		**** files added to the
		**** file switcher
		***/
		int32_t AddFile(string FilePath);
		void CloseFile(size_t FileIdx);

		/**
		**** Allows file switcher to 
		**** invoke the parser
		***/
		xml::document* RunParser(scratchpad::xml::parser* Parser, size_t FileIdx);

	public:

		vector<file_idx> Files;

	};

	file_switcher::file_switcher()
	{
		/***
		**** FILE SWITCHER
		***** CONSTRUCTOR
		***/
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

	xml::document* file_switcher::RunParser(xml::parser* Parser, size_t FileIdx)
	{
		xml::document* Document = nullptr;

		if(!Files.empty() && 
		   FileIdx >= 0   && 
		   FileIdx < Files.size())
		{
			if(!Files[FileIdx].Parsed)
			{
				Document = Parser->Parse(Files[FileIdx].Source);

				if(Document)
				{
					Files[FileIdx].Parsed = true;
					Files[FileIdx].Document = Document;
				}
			}
		}

		return Document;
	}

	int32_t file_switcher::AddFile(string FilePath)
	{
		if(!FilePath.empty())
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
				*** BY DEFAULT FILE SHOULD BE
				*** OPEN AFTER CONSTRUCTION
				**/
				new xml::source(FilePath),

				/**
				*** DEFAULT
				*** DOCUMENT PTR
				**/
				nullptr,

				/**
				*** DEFAULT
				*** PARSED STATE
				**/
				false
			};

			if(NewFileIdx.Source->File.is_open())
			{
				NewFileIdx.Source->ReadAll();

				if(NewFileIdx.Source->FileSize > 0)
				{
					Files.push_back(NewFileIdx);

					return Files.size() - 1;
				}
			}
		}

		return -1;
	}

	void file_switcher::CloseFile(size_t FileIdx)
	{
		if(!Files.empty())
		{
			size_t FileCount = Files.size();

			if(FileIdx > 0 && FileIdx < FileCount)
			{

				// dealloc memory
				delete Files[FileIdx].Source;
				Files[FileIdx].Source = nullptr;

			}
		}
	}
}

int main(int ArgCount, char* ArgV[])
{
	/***
	**** PRIMARY XML FILE SWITCHER FOR MAINTAINING
	**** XML SOURCE & DOCUMENT FILES
	***/
	scratchpad::file_switcher FileSwitcher;

	int32_t BookFileIdx = -1;
	int32_t SupplierIdx = -1;
	int32_t CustomerIdx = -1;
	int32_t OrdersIdx = -1;

	BookFileIdx = FileSwitcher.AddFile(scratchpad::XMLSourceFiles[0].data());
	SupplierIdx = FileSwitcher.AddFile(scratchpad::XMLSourceFiles[1].data());
	CustomerIdx = FileSwitcher.AddFile(scratchpad::XMLSourceFiles[2].data());
	OrdersIdx   = FileSwitcher.AddFile(scratchpad::XMLSourceFiles[5].data());

	/***
	**** PRIMARY SOURCE &
	**** PARSER code instances
	***/
	scratchpad::xml::parser XMLParser;

	/***
	**** XML Document
	***/
	scratchpad::xml::document* Document = nullptr;

	Document = FileSwitcher.RunParser(&XMLParser, BookFileIdx);

	return XMLParser.Diagnostics->ExitCode;
}