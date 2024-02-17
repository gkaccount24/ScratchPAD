#include "./include/xml_source.h"

namespace scratchpad
{
	xml_source::xml_source(const char* Path)
	{

	}

	xml_source::xml_source(xml_source&& RSource) noexcept:
		DiskPath(RSource.DiskPath),
		Buffer(move(RSource.Buffer))
	{
		RSource.DiskPath = nullptr;
		RSource.Buffer.clear();
		RSource.Buffer.shrink_to_fit();
	}
}
