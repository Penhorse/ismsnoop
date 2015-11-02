#include "ismsnoop/ismsnoop.h"

#include <cstring>
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
	std::string name;
};

const auto MAGIC_BYTE = 341;

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


enum class DataType
{
	None = 0,
	BackgroundImage,
	InfoText,
};

static bool looks_like_a_background_image(const std::vector<uint32_t> & buffer, int * next)
{
	if (buffer[0] == 65536
		&& buffer[1] == 65536
		&& buffer[2] == 1
		&& buffer[3] == 11
		&& buffer[4] == 0
		&& buffer[6] == buffer[5]
		&& buffer[7] == buffer[5]
		&& buffer[8] == buffer[5])
	{
		*next = 14;
		return true;
	}

	if (buffer[0] == 0
		&& buffer[1] == 0
		&& buffer[2] == 0
		&& buffer[3] == 9
		&& buffer[4] == 2
		&& buffer[5] == 11
		&& buffer[11] == 0
		&& buffer[12] == 0
		&& buffer[13] == 1
		&& buffer[15] == 1)
	{
		*next = 16;
		return true;
	}

	return false;
}

static bool looks_like_the_info_text(const std::vector<uint32_t> & buffer, int * next)
{
	if (buffer[0] == 0
		&& buffer[1] == 0
		&& buffer[2] == 0
		&& buffer[3] == 131072
		&& buffer[4] == 16842752
		&& buffer[5] == 16843009)
	{
		*next = 9;
		return true;
	}

	return false;
}

DataType find_data(std::ifstream & ifs, int byte_offset)
{
	ifs.seekg(byte_offset, std::ios::cur);

	auto start = ifs.tellg();

	std::vector<uint32_t> buffer;

	for (int i = 0; i < 128; i++)
	{
		uint32_t x;
		ifs.read((char*)(&x), 4);
		buffer.push_back(x);
	}

	while (buffer.size() > 15)
	{
		int next = 0;

		if (looks_like_a_background_image(buffer, &next))
		{
			ifs.seekg(start + (std::streampos((next * 4) + ((128 - buffer.size()) * 4))), std::ios::beg);
			return DataType::BackgroundImage;
		}

		if (looks_like_the_info_text(buffer, &next))
		{
			ifs.seekg(start + (std::streampos((next * 4) + ((128 - buffer.size()) * 4))), std::ios::beg);
			return DataType::InfoText;
		}

		buffer.erase(buffer.begin());
	}

	return DataType::None;
}

DataType find_data(std::ifstream & ifs)
{
	const auto start = ifs.tellg();

	DataType result = find_data(ifs, 0);

	if (result == DataType::None)
	{
		ifs.seekg(start, std::ios::beg);

		result = find_data(ifs, 2);
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

	if(file_length < MAGIC_BYTE)
	{
		return nullptr;
	}

	const auto result = new ISMSnoopInstrument();

	ifs.seekg(MAGIC_BYTE, std::ios::beg);

	char magic_byte;

	ifs.get(magic_byte);

	if (magic_byte == 0)
	{
		// no panel icon
		result->panel_icon.width = 0;
		result->panel_icon.height = 0;
		result->panel_icon.depth = 0;

		ifs.seekg(3, std::ios::cur);
	}
	else
	{
		ifs.seekg(7, std::ios::cur);

		char panel_icon_file_name_length;

		ifs.get(panel_icon_file_name_length);

		ifs.seekg(48 + panel_icon_file_name_length, std::ios::cur);

		uint16_t panel_icon_width, panel_icon_height, panel_icon_depth;

		ifs.read((char*)(&panel_icon_width), 2);
		ifs.read((char*)(&panel_icon_height), 2);
		ifs.read((char*)(&panel_icon_depth), 2);

		result->panel_icon.width = panel_icon_width;
		result->panel_icon.height = panel_icon_height;
		result->panel_icon.depth = panel_icon_depth;

		const auto channels = panel_icon_depth / 8;
		const auto panel_icon_size = result->panel_icon.width * result->panel_icon.height * channels;

		for(auto i = 0; i < panel_icon_size; i++)
		{
			char byte;

			ifs.get(byte);

			result->panel_icon.bytes.push_back(byte);
		}
	}

	DataType data_type;

	while ((data_type = find_data(ifs)) != DataType::None)
	{
		switch (data_type)
		{
			case DataType::BackgroundImage:
			{
				uint32_t file_name_length;

				ifs.read((char*)(&file_name_length), 4);

				ifs.seekg(45 + file_name_length, std::ios::cur);

				uint16_t width, height, depth;

				ifs.read((char*)(&width), 2);
				ifs.read((char*)(&height), 2);
				ifs.read((char*)(&depth), 2);

				const auto channels = depth / 8;
				const auto image_size = width * height * channels;

				ifs.seekg(image_size, std::ios::cur);

				break;
			}

			case DataType::InfoText:
			{
				uint32_t name_length;

				ifs.read((char*)(&name_length), 4);

				std::vector<char> buffer(name_length);

				ifs.read(buffer.data(), name_length);

				result->name = std::string(buffer.begin(), buffer.end());

				return result;
			}

			default:
			{
				break;
			}
		}
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

void ismsnoop_get_name(ISMSnoopInstrument * instrument, char * dest, int * length)
{
	if (length)
	{
		*length = instrument->name.size();
	}

	if (dest)
	{
		strncpy(dest, instrument->name.c_str(), instrument->name.size());
		dest[instrument->name.size()] = 0;
	}
}
