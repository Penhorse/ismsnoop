#pragma once

#include "version_handler.h"

namespace ismsnoop
{

struct R600Handler : public VersionHandler
{
	int start() override { return 0x155; }
	bool looks_like_a_background_image(const std::vector<uint32_t> & buffer, int * next) override;
	bool looks_like_the_info_text(const std::vector<uint32_t> & buffer, int * next) override;
};
	
} // namespace ismsnoop
