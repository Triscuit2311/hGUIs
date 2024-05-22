#include "pch.h"

#include "animations.hpp"
#include "D2xOverlay.h"
#include "DiInputManager.h"
#include "hGUI.h"
#include "InputDefines.h"
#include "WindowsUtils.h"
#include "logging.h"

Renderer::D2DBitmapID splash_img;

bool b[20] = {false};
double dub = 99.0;
long i32 = 13;
double f64= 9.0;

long i32b = 1000;
double f64a = 99999.0;


bool show_menu = false;
bool use_desktop_blur = false; //broken in windows 11
bool block_inputs_in_menu = true;
bool exit_thread = false;

size_t selection = 0;
// Runs once at start-time, initializes all the windows, groups and controls for the GUI
void setup_gui(std::shared_ptr<h_gui::workspace> ws)
{
	auto win1 = ws->add_window(L"windonk 1", {100, 100});

	auto cat = win1->add_category(L"CAT1");
	auto sec = cat->add_section(L"Global", L"globeico.png");
	auto tab = sec->add_tab(L"General");

	auto grp = tab->add_group(L"Group1");

	grp->toggle(&b[12], L"Buddle");
	grp->toggle(&b[0], L"Toggle");

	 grp->modal_selection(&selection, L"Selected Option: %s",
	 	L"YEET", { L"op1", L"op2", L"op3" }, win1);

	// grp->toggle(&b[1], L"My");
	// grp->toggle(&b[2], L"Noggle");
	// grp->toggle(&b[3], L"Like");
	// grp->toggle(&b[4], L"Corn");
	// grp->toggle(&b[5], L"On a");
	// grp->toggle(&b[6], L"Coggle");
	//
	// grp = tab->add_group(L"Group1");
	// grp->toggle(&b[10], L"Huddle");
	// grp->toggle(&b[11], L"MY");
	// grp->toggle(&b[12], L"Buddle");
	// grp->toggle(&b[13], L"Like");
	// grp->toggle(&b[14], L"Corn");
	// grp->toggle(&b[15], L"In a");
	// grp->toggle(&b[16], L"Cuddle");
	//
	// grp = tab->add_group(L"Group2");
	// grp->slider_double(&f64, -10, 10, L"Slip Speed: %.2lf M/s", [](long i) {});
	// grp->slider_long(&i32, -120, 120, L"Slide %d mph", [](long i) {});
	//
	//
	// grp = tab->add_group(L"Group3");
	// grp->toggle(&b[14], L"Bingo");
	// grp->toggle(&b[15], L"Bango");
	// grp->toggle(&b[16], L"Bongo");
	// grp->label(L"I don't wanna");
	// grp->label(L"Leave the congo");
	//
	//
	//
	// tab = sec->add_tab(L"Advanced");
	// grp = tab->add_group(L"Group1");
	// grp->slider_double(&f64a, -100, 1000000, L"Double A: %.2lf", [](long i) {});
	// grp->slider_long(&i32b, 0, 99999999999999, L"Long B: %ld", [](long i) {});
	// grp = tab->add_group(L"Group1");
	// grp->slider_double(&f64a, -100, 1000000, L"(also) Double A: %.2lf", [](long i) {});
	// grp->slider_long(&i32b, 0, 99999999999999, L"(also) Long B: %ld", [](long i) {});
	//
	//
	//
	//
	//
	// cat = win1->add_category(L"CAT1");
	// sec = cat->add_section(L"Visuals", L"eye_1_ico.png");
	// tab = sec->add_tab(L"General");
	// grp = tab->add_group(L"Group1");
	// grp->label(L"Nothing!");
	// grp = tab->add_group(L"Group1");
	// grp->label(L"Nothing!");	grp = tab->add_group(L"Group1");
	// grp->label(L"Nothing!");
	// sec = cat->add_section(L"Item ESP", L"eye_2_ico.png");
	// tab = sec->add_tab(L"General");
	// grp = tab->add_group(L"Group1");
	// grp->label(L"Nothing!");
	// grp = tab->add_group(L"Group1");
	// grp->label(L"Nothing!");
	// grp = tab->add_group(L"Group1");
	// grp->label(L"Nothing!");
	// tab = sec->add_tab(L"Advanced");
	// grp = tab->add_group(L"Group1");
	// grp->label(L"Nothing!");

}


// Runs once at start-time, does all resource loading from disk
void load_resources(Renderer::D2DxOverlay* renderer)
{
	//renderer->LoadSystemFontAsDefault(L"Jetbrains MONO"); // TODO: Load font from bytes
	renderer->LoadSystemFontAsDefault(L"Segoe UI"); // TODO: Load font from bytes
	splash_img = h_gui::globals::gui->create_resource_img(L"dealwithit_glow.png");
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
	renderer->DrawStringC(L"Triscuit2311", 8.0f, {10, 10}, {1, 1, 1, 1});
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
		// renderer->DrawBitmap(splash_img,
		//                      {
		// 	                     width / 2.0f - 128.0f, height / 2.0f - 128.0f, width / 2.0f + 128.0f,
		// 	                     height / 2.0f + 128.0f
		//                      }, curr_opac);
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
		renderer->SetSolidColor({0,0,0,0});

		// Setup GUI
		h_gui::globals::gui = std::make_unique<h_gui::gui_manager>(renderer, inputs);
		h_gui::globals::invoker = std::make_unique<h_gui::async_invoker>();


		load_resources(renderer);

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
