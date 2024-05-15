#include "pch.h"

#include "animations.hpp"
#include "D2xOverlay.h"
#include "DiInputManager.h"
#include "hGUI.h"
#include "InputDefines.h"
#include "WindowsUtils.h"
#include "logging.h"

Renderer::D2DBitmapID splash_img;

bool b;
double dub = 99.0;
long i32 = 123;


bool show_menu = true;
bool use_desktop_blur = false;
bool block_inputs_in_menu = false;
bool exit_thread = false;

// Runs once at start-time, initializes all the windows, groups and controls for the GUI
void setup_gui(std::shared_ptr<h_gui::workspace> ws)
{
	auto win1 = ws->add_window(L"windonk 1", { 100, 100 });

	auto c1 = win1->add_category(L"CAT1");

	auto s1 = c1->add_section(L"Sec1");
	auto t1 = s1->add_tab(L"TAB1");
	auto g1 = t1->add_group(L"asd");
	g1->button(L"BUTTON_WITH_ACTION", []() {LOG("BUTTON PRESSED"); });
	g1->button(L"BUTTON_NO_ACTION");

	// g1->label(L"yeet");
	 g1->toggle(&b, L"Too00gle");

	// auto g2 = t1->add_group(L"asd");
	// g2->slider_long(&i32, -10, 10, L"Slippidy", [](long i){});



	auto t2 = s1->add_tab(L"TAB2");
	auto t3 = s1->add_tab(L"TAB3");

	auto s2 = c1->add_section(L"Sec2");
	auto t4 = s2->add_tab(L"TAB1");
	auto t5 = s2->add_tab(L"TAB2");



	auto c2 = win1->add_category(L"CAT2");

	auto s3 = c2->add_section(L"Sec1");
	auto _t1 = s3->add_tab(L"TAB1");
	auto _t2 = s3->add_tab(L"TAB2");
	auto _t3 = s3->add_tab(L"TAB3");
	auto _t4 = s3->add_tab(L"TAB4");
	auto _t5 = s3->add_tab(L"TAB5");
	auto _t6 = s3->add_tab(L"TAB6");
	auto _t7 = s3->add_tab(L"TAB7");
	auto _t8 = s3->add_tab(L"TAB8");
	auto _t9 = s3->add_tab(L"TAB9");


	// c1->add_section(L"Sec3");
	// auto c2 =win1->add_category(L"CAT2");
	// c2->add_section(L"Sec1");
	// c2->add_section(L"Sec2");
	// c2->add_section(L"Sec3");
	// auto c3 = win1->add_category(L"CAT3");
	// c3->add_section(L"Sec1");
	// c3->add_section(L"Sec2");
	// c3->add_section(L"Sec3");
	//
	// grp1->toggle(&b, L"TOGGLE", []() { LOG("ENABLED"); }, []() { LOG("DISABLED"); });
	//
	// grp1->button(L"Butt", []() { LOG("Action!"); });
	//
	// grp1->slider_double(&dub, 0, 200, L"Slider f64: %.1lf", [](const double n)
	// {
	// 	LOG("dub: %.2f", n);
	// });
	//
	// grp1->slider_long(&i32, 0, 200, L"Slider i32: %ld", [](const long n) { LOG("long: %ld", n); });
}



// Runs once at start-time, does all resource loading from disk
void load_resources(Renderer::D2DxOverlay* renderer)
{
	auto path = std::filesystem::temp_directory_path().parent_path().parent_path();
	path /= "hGUI";
	if (!std::filesystem::exists(path))
	{
		std::filesystem::create_directories(path);
		// TODO: Copy images here? or maybe just skip all this and load from byte array.
	}
	renderer->LoadSystemFontAsDefault(L"Jetbrains MONO"); // TODO: Load font from bytes
	splash_img = renderer->CreateBitmapImageFromFile(path / "dealwithit_glow.png");
}


// Executed once per frame, before any rendering
void pre_render(std::shared_ptr<DiInputManager> inputs, Renderer::D2DxOverlay* renderer)
{
	if (inputs->IsInputJustReleased(DiInputManager::vKb_END))
	{
		Renderer::D2DxOverlay::exit = true;
		exit_thread = true;
	}


	if (inputs->IsInputJustReleased(DiInputManager::vKb_INSERT))
	{
		show_menu = !show_menu;
		if (use_desktop_blur) { renderer->ToggleAcrylicEffect(show_menu); }
		if (block_inputs_in_menu) { renderer->SetInputInterception(show_menu); }
	}
}

// Draw directly to the screen BEHIND the main GUI
void render_direct_pre(UINT32 width, UINT32 height, LPPOINT cur_pos, Renderer::D2DxOverlay* renderer)
{
	renderer->DrawStringC(L"Triscuit2311", 8.0f, { 10,10 }, {1,1,1,1});
}

// Draw the main GUI and gui components, only modify if needed
// Note: all components (windows, groups, controls) are rendered automatically
void render_gui(UINT32 width, UINT32 height, LPPOINT cur_pos)
{
	if (show_menu)
	{
		h_gui::globals::gui->render(width, height, 0, cur_pos);
	}
}

// Draw directly to the screen ON TOP of the main GUI
void render_direct_post(UINT32 width, UINT32 height, LPPOINT cur_pos, const Renderer::D2DxOverlay* renderer)
{
	// Splash Screen
	{
		static float target_opac = 1.1f;
		static float curr_opac = 0.0f;
		static float alpha = 0.01f;
		if (curr_opac < -0.1f) { return; }
		curr_opac = anim::lerp(curr_opac, target_opac, alpha);
		if (curr_opac > 1.0f)
		{
			target_opac = -2.f;

		}
		renderer->DrawBitmap(splash_img,
			{
				width / 2.0f - 128.0f, height / 2.0f - 128.0f, width / 2.0f + 128.0f,
				height / 2.0f + 128.0f
			}, curr_opac);
	}
}

// Executed once per frame, after all rendering is complete
void post_render(std::shared_ptr<DiInputManager> inputs)
{
}


void init_once(Renderer::D2DxOverlay*& renderer, std::shared_ptr<DiInputManager>& inputs)
{
	static bool once = false;
	if (!once)
	{
		inputs = std::make_unique<DiInputManager>();
		inputs->Init(renderer->GetTargetHwndRef());
		renderer->SetSolidColor({ 0.8f, 0.8f, 0.8f, 1 });

		load_resources(renderer);

		// Setup GUI
		h_gui::globals::gui = std::make_unique<h_gui::gui_manager>(renderer, inputs);
		h_gui::globals::invoker = std::make_unique<h_gui::async_invoker>();

		auto ws1 = h_gui::globals::gui->add_workspace();
		setup_gui(ws1);

		// Spawn invoke thread after func map is populated
		h_gui::globals::invoker->run();

		if (use_desktop_blur) { renderer->ToggleAcrylicEffect(show_menu); }
		if (block_inputs_in_menu) { renderer->SetInputInterception(show_menu); }

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


int WINAPI wWinMain(_In_ HINSTANCE, _In_opt_ HINSTANCE, _In_ LPWSTR, _In_ int)
{
	Renderer::RunOverlayThread(render_tick);

	while (!exit_thread) { std::this_thread::sleep_for(std::chrono::seconds(1)); }
	h_gui::globals::invoker->stop();

	return 0;
}
