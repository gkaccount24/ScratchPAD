#include "xml_document.h"
#include "xml_reader.h"

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

	return ExitCode;
}