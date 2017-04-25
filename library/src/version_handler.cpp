#include "version_handler.h"

namespace ismsnoop
{
	
void VersionHandler::find_panel_icon(std::ifstream & ifs)
{
	ifs.seekg(start(), std::ios::beg);
}

} // namespace ismsnoop
