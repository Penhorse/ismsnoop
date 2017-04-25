#include "r611_handler.h"

namespace ismsnoop
{
	
bool R611Handler::looks_like_a_background_image(const std::vector<uint32_t> & buffer, int * next)
{
	return false;
}

bool R611Handler::looks_like_the_info_text(const std::vector<uint32_t> & buffer, int * next)
{
	return false;
}

} // namespace ismsnoop
