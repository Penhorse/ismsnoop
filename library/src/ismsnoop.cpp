#include "ismsnoop/ismsnoop.h"

#include <fstream>
#include <iostream>
#include <vector>

struct ISMSnoopInstrumentImage
{
	int width;
	int height;
	int depth;
	std::vector<char> bytes;
};

struct ISMSnoopInstrument
{
	ISMSnoopInstrumentImage panel_icon;
};

const auto PANEL_ICON_FILE_NAME_LENGTH_BYTE = 349;

template <class T>
T swap_endian(T x)
{
	T result;

	unsigned char *dst = (unsigned char *)(&result);
	unsigned char *src = (unsigned char *)(&x);

	for(unsigned int i = 0; i < sizeof(T); i++)
	{
		dst[i] = src[sizeof(T) - 1 - i];
	}

	return result;
}

ISMSnoopInstrument * ismsnoop_open(const char * path)
{
	std::ifstream ifs(path, std::ios_base::in | std::ios_base::binary);

	if(!ifs.good())
	{
		return nullptr;
	}

	ifs.seekg(0, std::ios::end);

	const auto file_length = ifs.tellg();

	if(file_length < PANEL_ICON_FILE_NAME_LENGTH_BYTE)
	{
		return nullptr;
	}

	const auto result = new ISMSnoopInstrument();

	ifs.seekg(PANEL_ICON_FILE_NAME_LENGTH_BYTE, std::ios::beg);

	char panel_icon_file_name_length;

	ifs.get(panel_icon_file_name_length);

	ifs.seekg(47 + panel_icon_file_name_length, std::ios::cur);

	uint16_t panel_icon_width, panel_icon_height, panel_icon_depth;

	ifs.read((char*)(&panel_icon_width), 2);
	ifs.read((char*)(&panel_icon_height), 2);
	ifs.read((char*)(&panel_icon_depth), 2);

	panel_icon_width = swap_endian(panel_icon_width);
	panel_icon_height = swap_endian(panel_icon_height);
	panel_icon_depth = swap_endian(panel_icon_depth);

	result->panel_icon.width = panel_icon_width;
	result->panel_icon.height = panel_icon_height;
	result->panel_icon.depth = panel_icon_depth;

	ifs.seekg(1, std::ios::cur);

	const auto channels = panel_icon_depth / 8;
	const auto panel_icon_size = result->panel_icon.width * result->panel_icon.height * channels;

	for(auto i = 0; i < panel_icon_size; i++)
	{
		char byte;

		ifs.get(byte);

		result->panel_icon.bytes.push_back(byte);
	}

	return result;
}

void ismsnoop_close(ISMSnoopInstrument * instrument)
{
	delete instrument;
}

void ismsnoop_get_panel_icon_size(ISMSnoopInstrument * instrument, int * width, int * height, int * depth)
{
	*width = instrument->panel_icon.width;
	*height = instrument->panel_icon.height;
	*depth = instrument->panel_icon.depth;
}

void ismsnoop_get_panel_icon_bytes(ISMSnoopInstrument * instrument, char * dest)
{
	int i = 0;

	for(const auto byte : instrument->panel_icon.bytes)
	{
		dest[i++] = byte;
	}
}
