#include "ismsnoop/ismsnoop.h"

#include <cstring>
#include <fstream>
#include <iostream>
#include <memory>
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

const auto VERSION_INFO = 0x25;
const auto R600_START = 341;
const auto R603_START = 0x153;

enum class DataType
{
	None = 0,
	BackgroundImage,
	InfoText,
};

enum class FileVersion
{
	R600,
	R603,
	Latest = R603,
	Unknown,
};

struct VersionHandler
{
	virtual bool looks_like_a_background_image(const std::vector<uint32_t> & buffer, int * next) = 0;
	virtual bool looks_like_the_info_text(const std::vector<uint32_t> & buffer, int * next) = 0;
	virtual void find_panel_icon(std::ifstream & ifs) = 0;
};

struct R600Handler : public VersionHandler
{
	bool looks_like_a_background_image(const std::vector<uint32_t> & buffer, int * next) override;
	bool looks_like_the_info_text(const std::vector<uint32_t> & buffer, int * next) override;
	void find_panel_icon(std::ifstream & ifs) override;
};

struct R603Handler : public VersionHandler
{
	bool looks_like_a_background_image(const std::vector<uint32_t> & buffer, int * next) override;
	bool looks_like_the_info_text(const std::vector<uint32_t> & buffer, int * next) override;
	void find_panel_icon(std::ifstream & ifs) override;
};

using LatestHandler = R603Handler;

bool R600Handler::looks_like_a_background_image(const std::vector<uint32_t> & buffer, int * next)
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
		&& (buffer[2] == 0 || buffer[2] == 65536)
		&& (buffer[3] == 9 || buffer[3] == 65536)
		&& (buffer[4] == 2 || buffer[4] == 257)
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

bool R600Handler::looks_like_the_info_text(const std::vector<uint32_t> & buffer, int * next)
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

void R600Handler::find_panel_icon(std::ifstream & ifs)
{
	ifs.seekg(R600_START, std::ios::beg);
}

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
		&& (buffer[1] == 524288 || buffer[1] == 0)
		&& (buffer[2] == 524288 || buffer[2] == 0)
		&& buffer[3] == 131072
		&& buffer[4] == 16842752
		&& buffer[5] == 16843009)
	{
		*next = 9;
		return true;
	}

	return false;
}

void R603Handler::find_panel_icon(std::ifstream & ifs)
{
	ifs.seekg(R603_START, std::ios::beg);
}

static DataType find_data(std::ifstream & ifs, int byte_offset, std::shared_ptr<VersionHandler> handler)
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

		if (handler->looks_like_a_background_image(buffer, &next))
		{
			ifs.seekg(start + (std::streampos((next * 4) + ((128 - buffer.size()) * 4))), std::ios::beg);
			return DataType::BackgroundImage;
		}

		if (handler->looks_like_the_info_text(buffer, &next))
		{
			ifs.seekg(start + (std::streampos((next * 4) + ((128 - buffer.size()) * 4))), std::ios::beg);
			return DataType::InfoText;
		}

		buffer.erase(buffer.begin());
	}

	return DataType::None;
}

static DataType find_data(std::ifstream & ifs, std::shared_ptr<VersionHandler> handler)
{
	const auto start = ifs.tellg();

	DataType result = find_data(ifs, 0, handler);

	if (result == DataType::None)
	{
		ifs.seekg(start, std::ios::beg);

		result = find_data(ifs, 2, handler);
	}

	return result;
}

} // namespace ismsnoop

using namespace ismsnoop;

struct ISMSnoopInstrument
{
	InstrumentImage panel_icon;
	std::string name;
};

void ismsnoop_library_version(int * major, int * minor, int * patch, int * tweak)
{
	if(major) *major = PROJECT_VERSION_MAJOR;
	if(minor) *minor = PROJECT_VERSION_MINOR;
	if(patch) *patch = PROJECT_VERSION_PATCH;
	if(tweak) *tweak = PROJECT_VERSION_TWEAK;
}

FileVersion read_file_version(std::ifstream & ifs)
{
	ifs.seekg(VERSION_INFO, std::ios::beg);

	char version_byte;

	ifs.get(version_byte);

	switch (version_byte)
	{
		case 0x35: return FileVersion::R600;
		case 0x36: return FileVersion::R603;
		default: return FileVersion::Unknown;
	}
}

InstrumentImage read_panel_icon(std::ifstream & ifs)
{
	InstrumentImage result;

	char panel_icon_file_name_length;

	ifs.get(panel_icon_file_name_length);

	ifs.seekg(48 + panel_icon_file_name_length, std::ios::cur);

	uint16_t panel_icon_width, panel_icon_height, panel_icon_depth;

	ifs.read((char*)(&panel_icon_width), 2);
	ifs.read((char*)(&panel_icon_height), 2);
	ifs.read((char*)(&panel_icon_depth), 2);

	result.width = panel_icon_width;
	result.height = panel_icon_height;
	result.depth = panel_icon_depth;

	const auto channels = panel_icon_depth / 8;
	const auto panel_icon_size = result.width * result.height * channels;

	for (auto i = 0; i < panel_icon_size; i++)
	{
		char byte;

		ifs.get(byte);

		result.bytes.push_back(byte);
	}

	return result;
}

std::shared_ptr<VersionHandler> make_handler(FileVersion file_version)
{
	switch (file_version)
	{
		case FileVersion::R600:
		{
			return std::shared_ptr<VersionHandler>(new R600Handler);
		}
		default:
		{
			return std::shared_ptr<VersionHandler>(new LatestHandler);
		}
	}
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

	if(file_length < VERSION_INFO)
	{
		return nullptr;
	}

	const auto handler = make_handler(read_file_version(ifs));

	handler->find_panel_icon(ifs);

	const auto result = new ISMSnoopInstrument();

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

		result->panel_icon = read_panel_icon(ifs);
	}

	DataType data_type;

	//while (!ifs.eof())
	while ((data_type = find_data(ifs, handler)) != DataType::None)
	{
		//data_type = find_data(ifs);
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
