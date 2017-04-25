#pragma once

#include <memory>

#include "file_version.h"
#include "r600_handler.h"
#include "r603_handler.h"
#include "r611_handler.h"

namespace ismsnoop
{

using LatestHandler = R611Handler;
	
extern std::shared_ptr<VersionHandler> make_handler(FileVersion file_version);

} // namespace ismsnoop
