#include "r611_handler.h"

namespace ismsnoop
{
	
bool R611Handler::looks_like_a_background_image(const std::vector<uint32_t> & buffer, int * next)
{
	if (buffer[0] == 0
		&& buffer[1] == 9
		&& buffer[2] == 2
		&& buffer[3] == 11
		&& buffer[10] == 0
		&& buffer[11] == 3
		&& buffer[12] == 1
		&& buffer[14] == 1)
	{
		*next = 15;
		return true;
	}

	if (buffer[0] == 65536
		&& buffer[1] == 65536
		&& buffer[2] == 1
		&& buffer[3] == 11
		&& buffer[4] == 0
		&& buffer[6] == buffer[5]
		&& buffer[7] == buffer[5]
		&& buffer[8] == buffer[5])
	{
		*next = 15;
		return true;
	}

	return false;
}

bool R611Handler::looks_like_the_info_text(const std::vector<uint32_t> & buffer, int * next)
{
	if (buffer[0] == 262144
		&& buffer[1] == 524288
		&& buffer[2] == 524288
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
