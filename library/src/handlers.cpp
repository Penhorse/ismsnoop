#include "handlers.h"

namespace ismsnoop
{

std::shared_ptr<VersionHandler> make_handler(FileVersion file_version)
{
	switch (file_version)
	{
		case FileVersion::R600:
		{
			return std::shared_ptr<VersionHandler>(new R600Handler);
		}
		case FileVersion::R603:
		{
			return std::shared_ptr<VersionHandler>(new R603Handler);
		}
		case FileVersion::R611:
		{
			return std::shared_ptr<VersionHandler>(new R611Handler);
		}
		default:
		{
			return std::shared_ptr<VersionHandler>(new LatestHandler);
		}
	}
}
	
} // namespace ismsnoop
