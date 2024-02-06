#include <xml_reader.h>
#include <win32_io.h>

#include <iostream>

using std::cout;
using std::endl;

enum exit_code
{
	Success			= 0,

	// ERRORS
	FailedToOpenDoc = -100,
	FailedToReadDoc = -101
};

#define SAMPLE_FILE_COUNT 6

int main(int ArgCount, char* ArgV[])
{
	exit_code ExitCode = Success;

	xml_doc* Doc = nullptr;

	Doc = xml_doc::CreateAndOpen("Z:\\data\\xml samples\\books.xml");

	if(!Doc)
	{
		ExitCode = FailedToOpenDoc;
	}
	else
	{
		xml_reader Reader;

		if(!Reader.Read(Doc))
		{
			ExitCode = FailedToReadDoc;
		}
		else
		{
			// do stuff with the xml doc

		}
	}

	return ExitCode;
}