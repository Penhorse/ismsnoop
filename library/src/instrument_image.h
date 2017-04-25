#pragma once

#include <vector>

namespace ismsnoop
{
	
struct InstrumentImage
{
	int width;
	int height;
	int depth;
	std::vector<char> bytes;
};

} // namespace ismsnoop
