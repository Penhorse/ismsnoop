#include "ismsnoop/ismsnoop.h"
#include "data_type.h"
#include "file_version.h"
#include "handlers.h"
#include "instrument_image.h"

#include <cstring>
#include <iostream>
#include <memory>

namespace ismsnoop
{

const auto VERSION_INFO = 0x24;

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

FileVersion read_file_version(std::ifstream & ifs)
{
	ifs.seekg(VERSION_INFO, std::ios::beg);

	uint32_t version_bytes;

	ifs.read((char*)(&version_bytes), sizeof(version_bytes));

	switch (version_bytes)
	{
		case 825242929: return FileVersion::R600;
		case 3811128881: return FileVersion::R603;
		case 3827905843: return FileVersion::R611;
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

} // namespace ismsnoop

struct ISMSnoopInstrument
{
	ismsnoop::InstrumentImage panel_icon;
	std::string name;
};

void ismsnoop_library_version(int * major, int * minor, int * patch, int * tweak)
{
	if(major) *major = PROJECT_VERSION_MAJOR;
	if(minor) *minor = PROJECT_VERSION_MINOR;
	if(patch) *patch = PROJECT_VERSION_PATCH;
	if(tweak) *tweak = PROJECT_VERSION_TWEAK;
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

	if(file_length < ismsnoop::VERSION_INFO)
	{
		return nullptr;
	}

	const auto handler = make_handler(ismsnoop::read_file_version(ifs));

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

		result->panel_icon = ismsnoop::read_panel_icon(ifs);
	}

	ismsnoop::DataType data_type;

	//while (!ifs.eof())
	while ((data_type = find_data(ifs, handler)) != ismsnoop::DataType::None)
	{
		//data_type = find_data(ifs);
		switch (data_type)
		{
			case ismsnoop::DataType::BackgroundImage:
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

			case ismsnoop::DataType::InfoText:
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
