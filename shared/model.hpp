#pragma once

namespace model {

	struct foo { bool stale = false; float a; float b; float c; float d; };


	struct menu_settings
	{
		uint8_t some_str[128] = "";
	} inline g_menu_settings;

	struct client_settings
	{
		bool test_print = false;
	} inline g_client_settings;


}
