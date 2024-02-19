
#include "xml_source.h"
#include "xml_document.h"

#include <fstream>
#include <sstream>
#include <iostream>

using std::fstream;
using std::stringstream;

using std::cout;
using std::cin;

enum class exit_code
{
	Success			= 0,
	FailedToOpenDoc = -100,
	FailedToReadDoc = -101
};

int main(int ArgCount, char* ArgV[])
{
	string DiskPath("Z:\\data\\xml samples\\books.xml");
	fstream SourceFile(DiskPath);

	scratchpad::xml_source XMLSource(move(SourceFile), DiskPath);
	scratchpad::xml_document* XMLDoc = XMLSource.Parse();

	return 0;
}