#pragma once

#include <memory>

#include "r600_handler.h"
#include "r603_handler.h"
#include "r611_handler.h"

namespace ismsnoop
{

using LatestHandler = R611Handler;
	
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
