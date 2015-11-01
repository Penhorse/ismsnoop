#include <functional>

#define CATCH_CONFIG_MAIN
#include <catch.hpp>

#include <ismsnoop/ismsnoop.h>
#include <rtw/dynamic_library.hpp>

using Sig_ismsnoop_open = ISMSnoopInstrument*(const char*);
using Sig_ismsnoop_close = void(ISMSnoopInstrument*);
using Sig_ismsnoop_get_panel_icon_size = void(ISMSnoopInstrument*, int*, int*, int*);
using Sig_ismsnoop_get_panel_icon_bytes = void(ISMSnoopInstrument*, char*);

SCENARIO("ism files can be opened and information can be retrieved", "[test]")
{
	rtw::DynamicLibrary lib_ismsnoop(rtw::dylib::get_filename("ismsnoop"));

	WHEN("the library is loaded")
	{
		REQUIRE(lib_ismsnoop.load());

		THEN("all the functions can be retrieved")
		{
			const auto ismsnoop_open = lib_ismsnoop.get_function<Sig_ismsnoop_open>("ismsnoop_open");
			const auto ismsnoop_close = lib_ismsnoop.get_function<Sig_ismsnoop_close>("ismsnoop_close");
			const auto ismsnoop_get_panel_icon_size = lib_ismsnoop.get_function<Sig_ismsnoop_get_panel_icon_size>("ismsnoop_get_panel_icon_size");
			const auto ismsnoop_get_panel_icon_bytes = lib_ismsnoop.get_function<Sig_ismsnoop_get_panel_icon_bytes>("ismsnoop_get_panel_icon_bytes");

			REQUIRE(ismsnoop_open);
			REQUIRE(ismsnoop_close);
			REQUIRE(ismsnoop_get_panel_icon_size);
			REQUIRE(ismsnoop_get_panel_icon_bytes);

			AND_THEN("a test ism can be opened")
			{
				const auto ism_test = ismsnoop_open("isms/1 In 2 Out Switch.ism");

				REQUIRE(ism_test);

				ismsnoop_close(ism_test);
			}

			AND_THEN("a test ism can be opened")
			{
				const auto ism_test = ismsnoop_open("isms/Acid Filter.ism");

				REQUIRE(ism_test);

				ismsnoop_close(ism_test);
			}

			AND_THEN("a test ism can be opened")
			{
				const auto ism_test = ismsnoop_open("isms/Thrasher v1.1.ism");

				REQUIRE(ism_test);

				ismsnoop_close(ism_test);
			}

			AND_THEN("another ism can be opened")
			{
				const auto ism_test = ismsnoop_open("isms/WAYO-WAYO v1.2.ism");

				REQUIRE(ism_test);

				AND_THEN("the panel icon size is correct")
				{
					int width = 0, height = 0, depth = 0;

					ismsnoop_get_panel_icon_size(ism_test, &width, &height, &depth);

					REQUIRE(width == 115);
					REQUIRE(height == 82);
					REQUIRE(depth == 32);
				}

				ismsnoop_close(ism_test);
			}


		}
	}
}
