#include <functional>

#define CATCH_CONFIG_MAIN
#include <catch.hpp>

#include <ismsnoop/ismsnoop.h>
#include <rtw/dynamic_library.hpp>

using Sig_ismsnoop_open = ISMSnoopInstrument*(const char*);
using Sig_ismsnoop_close = void(ISMSnoopInstrument*);
using Sig_ismsnoop_get_panel_icon_size = void(ISMSnoopInstrument*, int*, int*, int*);
using Sig_ismsnoop_get_panel_icon_bytes = void(ISMSnoopInstrument*, char*);
using Sig_ismsnoop_get_name = void(ISMSnoopInstrument*, char*, int*);

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
			const auto ismsnoop_get_name = lib_ismsnoop.get_function<Sig_ismsnoop_get_name>("ismsnoop_get_name");

			REQUIRE(ismsnoop_open);
			REQUIRE(ismsnoop_close);
			REQUIRE(ismsnoop_get_panel_icon_size);
			REQUIRE(ismsnoop_get_panel_icon_bytes);
			REQUIRE(ismsnoop_get_name);
			
			AND_WHEN("we try to open '1 In 2 Out Switch.ism'")
			{
				const auto ism_test = ismsnoop_open("isms/1 In 2 Out Switch.ism");

				THEN("it opens successfully")
				{
					REQUIRE(ism_test);
				}

				ismsnoop_close(ism_test);
			}

			AND_WHEN("we try to open 'Acid Filter.ism'")
			{
				const auto ism_test = ismsnoop_open("isms/Acid Filter.ism");

				THEN("it opens successfully")
				{
					REQUIRE(ism_test);
				}

				ismsnoop_close(ism_test);
			}

			AND_WHEN("we try to open 'Thrasher v1.1.ism'")
			{
				const auto ism_test = ismsnoop_open("isms/Thrasher v1.1.ism");

				THEN("it opens successfully")
				{
					REQUIRE(ism_test);
				}

				ismsnoop_close(ism_test);
			}

			AND_WHEN("we try to open 'OSC-Bargain Pulse.ism'")
			{
				const auto ism_test = ismsnoop_open("isms/OSC-Bargain Pulse.ism");

				THEN("it opens successfully")
				{
					REQUIRE(ism_test);
				}

				ismsnoop_close(ism_test);
			}

			AND_WHEN("we try to open 'Saturator.ism'")
			{
				const auto ism_test = ismsnoop_open("isms/Saturator.ism");

				THEN("it opens successfully")
				{
					REQUIRE(ism_test);
				}

				AND_THEN("we can retrieve the name")
				{
					int length;

					ismsnoop_get_name(ism_test, nullptr, &length);

					const auto c_name = new char[length + 1];
					
					ismsnoop_get_name(ism_test, c_name, &length);

					std::string name(c_name);

					delete[] c_name;

					REQUIRE(name == "Saturator");
					
				}

				ismsnoop_close(ism_test);
			}
			

			AND_WHEN("we try to open 'Tiny Ring Mod.ism'")
			{
				const auto ism_test = ismsnoop_open("isms/Tiny Ring Mod.ism");

				THEN("it opens successfully")
				{
					REQUIRE(ism_test);
				}

				AND_THEN("we can retrieve the name")
				{
					int length;

					ismsnoop_get_name(ism_test, nullptr, &length);

					const auto c_name = new char[length + 1];

					ismsnoop_get_name(ism_test, c_name, &length);

					std::string name(c_name);

					delete[] c_name;

					REQUIRE(name == "Tiny Ring Mod");

				}

				ismsnoop_close(ism_test);
			}
		}
	}
}
