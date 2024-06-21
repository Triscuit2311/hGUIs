#include "pch.h"

#include "animations.hpp"
#include "D2xOverlay.h"
#include "DiInputManager.h"
#include "hGUI.h"
#include "InputDefines.h"
#include "WindowsUtils.h"
#include "../shared/shared_mem.hpp"

#include "../shared/model.hpp"




std::vector<model::foo> data(10);

shared_memory_vec<model::foo> shm(L"SharedMemory", 10);
shared_memory_single<model::menu_settings> menu_shm(L"SHM_MENU_SETTINGS");
shared_memory_single<model::client_settings> client_shm(L"SHM_CLIENT_SETTINGS");

Renderer::D2DBitmapID splash_img;

bool b[20] = {false};
double dub = 99.0;
long i32 = 13;
double f64 = 9.0;

long i32b = 1000;
double f64a = 99999.0;


bool use_splash = false;
bool exit_thread = false;

D2D1_COLOR_F myColor = {0.7f, 0.2f, 0.8f, 0.65f};
DiInputManager::DiInput myHotkey = DiInputManager::vKb_LALT;

size_t selection = 0;


struct example_settings
{
	struct _visuals
	{
		bool enabled = true;

		struct _esp
		{
			struct _loot
			{
			} loot;

			struct _player
			{
			} player;

			struct _ai
			{
			} ai;
		} esp;

		struct _crosshair
		{
		} crosshair;

		struct _misc
		{
		} misc;
	} vis;
} g_settings;



std::wstring s2ws(char* cStr) {
	std::wstring wStr;
	wStr.reserve(strlen(cStr) + 1);
	for (int i = 0; i < strlen(cStr); i++) {
		wStr += wchar_t(cStr[i]);
	}
	wStr += L'\0'; // Add the null-terminator
	return wStr;
}

// Runs once at start-time, initializes all the windows, groups and controls for the GUI
void setup_gui(const std::shared_ptr<h_gui::workspace>& ws)
{
	const auto win1 = ws->add_window(L"hGUI Prototype v0.63b", {500, 200});

	const auto cat1 = win1->add_category(L"CAT1");
	const auto aim = cat1->add_section(L"Aimbot", L"icons/target.png");


	const auto aim_gen = aim->add_tab(L"General");
	const auto grp = aim_gen->add_group(L"Group1");
	grp->toggle(&model::g_client_settings.test_print, L"PRINT");
	grp->toggle(&b[0], L"Toggle");
	grp->slider_double(&f64, -10, 10, L"Slip Speed: %.2lf M/s", [](long i)
	{
	});
	grp->slider_long(&i32, -120, 120, L"Slide %d mph", [](long i)
	{
	});
	grp->modal_selection(&selection, L"Aim Bone: %s",
	                     L"Select Aim Bone", {L"Head", L"Neck", L"Chest", L"Pelvis", L"Wiener", L"Extremities"}, win1);
	grp->modal_color(&myColor, L"My Color", win1);
	grp->modal_hotkey(&myHotkey, L"My Hotkey: %s", L"My Hotkey", win1);


	auto aim_tar = aim->add_tab(L"Target Select");
	auto aim_adv = aim->add_tab(L"Advanced");

	cat1->add_section(L"ESP", L"icons/eye_scan.png");
	cat1->add_section(L"Loot Filter", L"icons/crown.png");
	cat1->add_section(L"Radar", L"icons/radar.png");

	auto cat2 = win1->add_category(L"CAT2");
	cat2->add_section(L"Miscellaneous", L"icons/magic_wand.png");
	cat2->add_section(L"Experimental", L"icons/test_tube.png");

	auto cat3 = win1->add_category(L"CAT3");
	cat3->add_section(L"Colors", L"icons/pallete.png");
	cat3->add_section(L"Hotkeys", L"icons/lightning.png");
	cat3->add_section(L"System", L"icons/settings.png");
}


// Runs once at start-time, does all resource loading from disk
void load_resources(Renderer::D2DxOverlay* renderer)
{
	//renderer->LoadSystemFontAsDefault(L"Jetbrains MONO"); // TODO: Load font from bytes
	renderer->LoadSystemFontAsDefault(L"Segoe UI"); // TODO: Load font from bytes
	splash_img = h_gui::globals::gui->create_resource_img(L"splash.png");
}


// Executed once per frame, before any rendering
void pre_render(std::shared_ptr<DiInputManager> inputs, Renderer::D2DxOverlay* renderer)
{
	if (inputs->IsInputJustReleased(DiInputManager::vKb_END))
	{
		Renderer::D2DxOverlay::exit = true;
		exit_thread = true;
	}

	//if (inputs->IsInputJustReleased(DiInputManager::vKb_INSERT))
	if(GetAsyncKeyState(VK_INSERT)&0x01)
	{
		h_gui::gui_manager::toggle_menu();
	}

	menu_shm.read(&model::g_menu_settings);
	client_shm.write(&model::g_client_settings);

}

// Draw directly to the screen BEHIND the main GUI
void render_direct_pre(UINT32 width, UINT32 height, LPPOINT cur_pos, Renderer::D2DxOverlay* renderer)
{
	renderer->DrawStringC(L"Triscuit2311", 8.0f, {10, 10}, {1, 1, 1, 1});

	renderer->DrawStringC(s2ws((char*) & model::g_menu_settings.some_str), 18.0f, {10, 110}, {1, 1, 1, 1});


	if (!model::g_menu_settings.visuals_to_front) {
		shm.read(data);
		for (const auto& item : data) {
			renderer->DrawLineC({ item.a, item.b }, { item.c, item.d }, 2, { 1,0,0,1 });
		}
	}
}

// Draw the main GUI and gui components, only modify if needed
// Note: all components (windows, groups, controls) are rendered automatically
void render_gui(UINT32 width, UINT32 height, LPPOINT cur_pos)
{
	h_gui::globals::gui->render(width, height, 0, cur_pos);
}

// Draw directly to the screen ON TOP of the main GUI
void render_direct_post(UINT32 width, UINT32 height, LPPOINT cur_pos, Renderer::D2DxOverlay* renderer)
{
	if (model::g_menu_settings.visuals_to_front) {
		shm.read(data);
		for (const auto& item : data) {
			renderer->DrawLineC({ item.a, item.b }, { item.c, item.d }, 2, { 1,0,0,1 });
		}
	}


	// Splash Screen
	{
		if (use_splash)
		{
			static bool set_once = false;

			if (!set_once)
			{
				h_gui::globals::gui->set_effects_only(true);
				set_once = true;
			}

			static float target_opac = 1.1f;
			static float curr_opac = 0.001f;
			static float alpha = 0.04f;
			if (curr_opac < 0.0001f)
			{
				use_splash = false;
				h_gui::globals::gui->set_show_menu(true);
			}
			curr_opac = anim::lerp(curr_opac, target_opac, alpha);
			if (curr_opac > 1.0f)
			{
				target_opac = -0.002f;
			}
			renderer->DrawBitmap(splash_img,
			                     {
				                     width / 2.0f - 512.0f, height / 2.0f - 512.0f, width / 2.0f + 512.0f,
				                     height / 2.0f + 512.0f
			                     }, curr_opac);
		}
	}
}

// Executed once per frame, after all rendering is complete
void post_render(std::shared_ptr<DiInputManager> inputs)
{

}


void init_once(Renderer::D2DxOverlay* renderer, std::shared_ptr<DiInputManager>& inputs)
{
	static bool once = false;
	if (!once)
	{
		inputs = std::make_unique<DiInputManager>();


		auto ref = renderer->GetTargetHwndRef();

		inputs->Init(ref);


		renderer->SetSolidColor({0, 0, 0, 0});


		// Setup GUI
		h_gui::globals::gui = std::make_unique<h_gui::gui_manager>(renderer, inputs);


		h_gui::globals::invoker = std::make_unique<h_gui::async_invoker>();


		load_resources(renderer);


		auto ws1 = h_gui::globals::gui->add_workspace();


		setup_gui(ws1);


		// Spawn invoke thread after func map is populated
		h_gui::globals::invoker->run();

		////
		once = true;
	}
}


void render_tick(UINT32 width, UINT32 height)
{
	static Renderer::D2DxOverlay* renderer = Renderer::D2DxOverlay::GetInstance();
	static LPPOINT cur_pos = new tagPOINT();
	static std::shared_ptr<DiInputManager> inputs;


	static auto last_time = std::chrono::steady_clock::now();
	auto curr_time = std::chrono::steady_clock::now();
	auto delta_time = curr_time - last_time;
	std::chrono::duration<double> delta_secs = curr_time - last_time;
	last_time = curr_time;

	init_once(renderer, inputs);

	inputs->UpdateDeviceStates();

	windows_utils::abs_curpos(*cur_pos);

	pre_render(inputs, renderer);
	render_direct_pre(width, height, cur_pos, renderer);
	render_gui(width, height, cur_pos);
	render_direct_post(width, height, cur_pos, renderer);
	post_render(inputs);

	inputs->RetainDeviceStates();
}


DWORD WINAPI hThread(LPVOID lpParam)
{
	Renderer::RunOverlayThread(render_tick);
	while (!exit_thread) { std::this_thread::sleep_for(std::chrono::seconds(1)); }
	h_gui::globals::invoker->stop();
	return 0;
}




int WINAPI wWinMain(_In_ HINSTANCE, _In_opt_ HINSTANCE, _In_ LPWSTR, _In_ int)
{
	//INIT_CONSOLE();




	hThread(nullptr);
	return 0;
}


BOOL WINAPI DllMain(
	HINSTANCE hinstDLL,
	DWORD fdwReason,
	LPVOID lpvReserved)
{
	if (fdwReason == DLL_PROCESS_ATTACH)
	{
		CreateThread(nullptr, 0, hThread, nullptr, 0, nullptr);
	}
	return TRUE;
}
