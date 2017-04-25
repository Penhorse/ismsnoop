#include "r603_handler.h"

namespace ismsnoop
{
	
bool R603Handler::looks_like_a_background_image(const std::vector<uint32_t> & buffer, int * next)
{
	if (buffer[0] == 65536
		&& buffer[1] == 65536
		&& buffer[2] == 1
		&& buffer[3] == 11
		&& buffer[4] == 0
		&& buffer[6] == buffer[5]
		&& buffer[7] == buffer[5]
		&& buffer[8] == buffer[5]
		&& buffer[9] == 0
		&& buffer[10] == 0
		&& buffer[11] == 3
		&& buffer[12] == 1
		&& buffer[13] == 3)
	{
		*next = 15;
		return true;
	}

	if ((buffer[0] == 0 || buffer[0] == 524288)
		&& (buffer[1] == 0 || buffer[1] == 524288)
		&& (buffer[2] == 0 || buffer[2] == 65536)
		&& (buffer[3] == 9 || buffer[3] == 65536)
		&& (buffer[4] == 2 || buffer[4] == 257)
		&& buffer[5] == 11
		&& buffer[11] == 0
		&& buffer[12] == 0
		&& buffer[13] == 3)
	{
		*next = 17;
		return true;
	}

	return false;
}

bool R603Handler::looks_like_the_info_text(const std::vector<uint32_t> & buffer, int * next)
{
	if ((buffer[0] == 262144 || buffer[0] == 0)
		&& buffer[3] == 131072
		&& buffer[4] == 16842752
		&& buffer[5] == 16843009)
	{
		*next = 9;
		return true;
	}

	return false;
}

} // namespace ismsnoop
