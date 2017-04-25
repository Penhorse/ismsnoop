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

struct ISMSnoopLib
{
	ISMSnoopLib();

	std::shared_ptr<rtw::DynamicLibrary> lib;
};

ISMSnoopLib::ISMSnoopLib()
	: lib(std::make_shared<rtw::DynamicLibrary>(rtw::dylib::get_filename("ismsnoop")))
{
	REQUIRE(lib->load());
}

struct ISMSnoop
{
	ISMSnoop(const ISMSnoopLib & lib);

	std::function<Sig_ismsnoop_open> open;
	std::function<Sig_ismsnoop_close> close;
	std::function<Sig_ismsnoop_get_panel_icon_size> get_panel_icon_size;
	std::function<Sig_ismsnoop_get_panel_icon_bytes> get_panel_icon_bytes;
	std::function<Sig_ismsnoop_get_name> get_name;
};

ISMSnoop::ISMSnoop(const ISMSnoopLib & lib)
	: open(lib.lib->get_function<Sig_ismsnoop_open>("ismsnoop_open"))
	, close(lib.lib->get_function<Sig_ismsnoop_close>("ismsnoop_close"))
	, get_panel_icon_size(lib.lib->get_function<Sig_ismsnoop_get_panel_icon_size>("ismsnoop_get_panel_icon_size"))
	, get_panel_icon_bytes(lib.lib->get_function<Sig_ismsnoop_get_panel_icon_bytes>("ismsnoop_get_panel_icon_bytes"))
	, get_name(lib.lib->get_function<Sig_ismsnoop_get_name>("ismsnoop_get_name"))
{
	REQUIRE(open);
	REQUIRE(close);
	REQUIRE(get_panel_icon_size);
	REQUIRE(get_panel_icon_bytes);
	REQUIRE(get_name);
}

class Instrument
{
public:
	Instrument(const ISMSnoop & ismsnoop, const std::string & path);
	~Instrument();

	std::string get_name() const;

private:

	const ISMSnoop * ismsnoop_;
	ISMSnoopInstrument * instrument_;
};

Instrument::Instrument(const ISMSnoop & ismsnoop, const std::string & path)
	: ismsnoop_(&ismsnoop)
	, instrument_(ismsnoop_->open(path.c_str()))
{
	REQUIRE(instrument_);
}

Instrument::~Instrument()
{
	ismsnoop_->close(instrument_);
}

std::string Instrument::get_name() const
{
	int length;

	ismsnoop_->get_name(instrument_, nullptr, &length);

	const auto c_name = new char[length + 1];
	
	ismsnoop_->get_name(instrument_, c_name, &length);

	std::string result(c_name);

	delete[] c_name;

	return result;
}

SCENARIO("the library can be loaded", "[library]")
{
	THEN("the library can be opened")
	{
		ISMSnoopLib lib;

		AND_THEN("all functions can be retrieved")
		{
			ISMSnoop ismsnoop(lib);
		}
	}
}

SCENARIO("R600 instruments can be read correctly", "[r600]")
{
	WHEN("the library is loaded")
	{
		ISMSnoopLib lib;
		ISMSnoop ismsnoop(lib);

		THEN("'1 In 2 Out Switch.ism' can be opened")
		{
			Instrument ism(ismsnoop, "isms/1 In 2 Out Switch.ism");
		}

		AND_THEN("'Acid Filter.ism' can be opened")
		{
			Instrument ism(ismsnoop, "isms/Acid Filter.ism");
		}

		AND_THEN("'Thrasher v1.1.ism' can be opened")
		{
			Instrument ism(ismsnoop, "isms/Thrasher v1.1.ism");
		}

		AND_THEN("'OSC-Bargain Pulse.ism' can be opened")
		{
			Instrument ism(ismsnoop, "isms/OSC-Bargain Pulse.ism");
		}

		AND_THEN("'Saturator.ism' can be opened")
		{
			Instrument ism(ismsnoop, "isms/Saturator.ism");

			AND_THEN("we can retrieve the name")
			{
				REQUIRE(ism.get_name() == "Saturator");
			}
		}

	}
}

/*
SCENARIO("Instruments with no background image can be read correctly", "[!mayfail]")
{
	WHEN("the library is loaded")
	{
		ISMSnoopLib lib;
		ISMSnoop ismsnoop(lib);

		THEN("'Tiny Ring Mod.ism' can be opened")
		{
			Instrument ism(ismsnoop, "isms/Tiny Ring Mod.ism");

			AND_THEN("we can retrieve the name")
			{
				REQUIRE(ism.get_name() == "Tiny Ring Mod");
			}
		}
	}
}
*/

SCENARIO("R603 instruments can be read correctly", "[r603]")
{
	WHEN("the library is loaded")
	{
		ISMSnoopLib lib;
		ISMSnoop ismsnoop(lib);

		THEN("'Fold.ism' can be opened")
		{
			Instrument ism(ismsnoop, "isms/Fold.ism");

			AND_THEN("we can retrieve the name")
			{
				REQUIRE(ism.get_name() == "Folds");
			}
		}

		AND_THEN("'SA MiniVerb v.1.2a.ism' can be opened")
		{
			Instrument ism(ismsnoop, "isms/SA MiniVerb v.1.2a.ism");

			AND_THEN("we can retrieve the name")
			{
				REQUIRE(ism.get_name() == "SA MiniVerb");
			}
		}

		AND_THEN("'SA_MidSide_v.1.1.ism' can be opened")
		{
			Instrument ism(ismsnoop, "isms/SA_MidSide_v.1.1.ism");

			AND_THEN("we can retrieve the name")
			{
				REQUIRE(ism.get_name() == "SA Mid/Side");
			}
		}

		AND_THEN("'BND v1.0.ism' can be opened")
		{
			Instrument ism(ismsnoop, "isms/BND v1.0.ism");

			AND_THEN("we can retrieve the name")
			{
				REQUIRE(ism.get_name() == "BND");
			}
		}

		AND_THEN("'Metaverb Red.ism' can be opened")
		{
			Instrument ism(ismsnoop, "isms/Metaverb Red.ism");

			AND_THEN("we can retrieve the name")
			{
				REQUIRE(ism.get_name() == "Metaverb Red");
			}
		}
	}
}

SCENARIO("R611 instruments can be read correctly", "[611]")
{
	WHEN("the library is loaded")
	{
		ISMSnoopLib lib;
		ISMSnoop ismsnoop(lib);

		THEN("'Maxted Dyn Rectifier _M_.ism' can be opened")
		{
			Instrument ism(ismsnoop, "isms/Maxted Dyn Rectifier _M_.ism");

			AND_THEN("we can retrieve the name")
			{
				REQUIRE(ism.get_name() == "Maxted (M)");
			}
		}

		AND_THEN("'Maxted Dyn Rectifier _S_.ism' can be opened")
		{
			Instrument ism(ismsnoop, "isms/Maxted Dyn Rectifier _S_.ism");

			AND_THEN("we can retrieve the name")
			{
				REQUIRE(ism.get_name() == "Maxted (S)");
			}
		}

		AND_THEN("'Royal Feedback Sat _M_.ism' can be opened")
		{
			Instrument ism(ismsnoop, "isms/Royal Feedback Sat _M_.ism");

			AND_THEN("we can retrieve the name")
			{
				REQUIRE(ism.get_name() == "Royal (M)");
			}
		}

		AND_THEN("'Royal Feedback Sat _S_.ism' can be opened")
		{
			Instrument ism(ismsnoop, "isms/Royal Feedback Sat _S_.ism");

			AND_THEN("we can retrieve the name")
			{
				REQUIRE(ism.get_name() == "Royal (S)");
			}
		}

		AND_THEN("'Kerans LP4 Dyn Shaper _M_.ism' can be opened")
		{
			Instrument ism(ismsnoop, "isms/Kerans LP4 Dyn Shaper _M_.ism");

			AND_THEN("we can retrieve the name")
			{
				REQUIRE(ism.get_name() == "Kerans (M)");
			}
		}

		AND_THEN("'Kerans LP4 Dyn Shaper _S_.ism' can be opened")
		{
			Instrument ism(ismsnoop, "isms/Kerans LP4 Dyn Shaper _S_.ism");

			AND_THEN("we can retrieve the name")
			{
				REQUIRE(ism.get_name() == "Kerans (S)");
			}
		}

		AND_THEN("'Laing LP3 Dyn Shaper _M_.ism' can be opened")
		{
			Instrument ism(ismsnoop, "isms/Laing LP3 Dyn Shaper _M_.ism");

			AND_THEN("we can retrieve the name")
			{
				REQUIRE(ism.get_name() == "Laing (M)");
			}
		}

		AND_THEN("'Laing LP3 Dyn Shaper _S_.ism' can be opened")
		{
			Instrument ism(ismsnoop, "isms/Laing LP3 Dyn Shaper _S_.ism");

			AND_THEN("we can retrieve the name")
			{
				REQUIRE(ism.get_name() == "Laing (S)");
			}
		}

		AND_THEN("'Margot Multicomb _M_.ism' can be opened")
		{
			Instrument ism(ismsnoop, "isms/Margot Multicomb _M_.ism");

			AND_THEN("we can retrieve the name")
			{
				REQUIRE(ism.get_name() == "Margot (M)");
			}
		}

		AND_THEN("'Margot Multicomb _S_.ism' can be opened")
		{
			Instrument ism(ismsnoop, "isms/Margot Multicomb _S_.ism");

			AND_THEN("we can retrieve the name")
			{
				REQUIRE(ism.get_name() == "Margot (S)");
			}
		}

		AND_THEN("'Wilder LP2 Dyn Shaper _M_.ism' can be opened")
		{
			Instrument ism(ismsnoop, "isms/Wilder LP2 Dyn Shaper _M_.ism");

			AND_THEN("we can retrieve the name")
			{
				REQUIRE(ism.get_name() == "Wilder (M)");
			}
		}

		AND_THEN("'Wilder LP2 Dyn Shaper _S_.ism' can be opened")
		{
			Instrument ism(ismsnoop, "isms/Wilder LP2 Dyn Shaper _S_.ism");

			AND_THEN("we can retrieve the name")
			{
				REQUIRE(ism.get_name() == "Wilder (S)");
			}
		}
	}
}

SCENARIO("Bezyeah Clock should load correctly", "[bezyeah][misc]")
{
	WHEN("the library is loaded")
	{
		ISMSnoopLib lib;
		ISMSnoop ismsnoop(lib);

		THEN("'INT-Bezyeah_Clock.ism' can be opened")
		{
			Instrument ism(ismsnoop, "isms/INT-Bezyeah_Clock.ism");

			AND_THEN("we can retrieve the name")
			{
				REQUIRE(ism.get_name() == "Bezyeah Clock");
			}
		}
	}
}
