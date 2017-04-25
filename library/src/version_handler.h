#pragma once

#include <fstream>
#include <vector>

namespace ismsnoop
{
	
struct VersionHandler
{
	void find_panel_icon(std::ifstream & ifs);

	virtual int start() = 0;
	virtual bool looks_like_a_background_image(const std::vector<uint32_t> & buffer, int * next) = 0;
	virtual bool looks_like_the_info_text(const std::vector<uint32_t> & buffer, int * next) = 0;
};

} // namespace ismsnoop
