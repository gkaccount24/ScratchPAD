#include "./include/xml_source.h"

namespace scratchpad
{
	xml_source::xml_source(const char* Path):
		Buffer(io_filebuffer(Path))
	{

	}

	xml_source::xml_source(xml_source&& RSource) noexcept:
		DiskPath(RSource.DiskPath),
		Buffer(move(RSource.Buffer))
	{
		RSource.DiskPath = nullptr;
	}

	xml_source::~xml_source()
	{
		DiskPath = nullptr;
	}
}
