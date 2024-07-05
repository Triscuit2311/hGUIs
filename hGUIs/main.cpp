#include "pch.h"

#include "animations.hpp"
#include "D2xOverlay.h"
#include "DiInputManager.h"
#include "hGUI.h"
#include "InputDefines.h"
#include "WindowsUtils.h"
#include "../shared/shared_mem.hpp"

#include "../shared/model.hpp"

model::humanoid g_enemy_array[model::ENEMY_ARRAY_SIZE];
shared_memory_array<model::humanoid, model::ENEMY_ARRAY_SIZE> g_enemy_shm(L"ENEMY_SHM");

shared_memory_single<model::menu_settings> menu_shm(L"SHM_MENU_SETTINGS");
shared_memory_single<model::client_settings> client_shm(L"SHM_CLIENT_SETTINGS");

Renderer::D2DBitmapID splash_img;

bool use_splash = false;
bool exit_thread = false;


namespace ESP_COLORS
{
	inline static D2D1_COLOR_F self{ .2f,.2f,.2f,1 };
	inline static D2D1_COLOR_F team{ .2f, .9f,.5f,1 };
	inline static D2D1_COLOR_F enemy{ .9f,.2f,.2f,1 };
	inline static D2D1_COLOR_F team_bot{ .2f,.5f,.9f,1 };
	inline static D2D1_COLOR_F enemy_bot{ .9f,.5f,.2f,1 };
}


float scale_on_dist(float distance, float min_dist, float max_dist, float min_scale, float max_scale) {
	if (distance < min_dist) { return max_scale; }
	if (distance > max_dist) { return min_scale; }

	distance = std::clamp(distance, min_dist, max_dist);
	float scale = min_scale + (distance - min_dist) * (max_scale - min_scale) / (max_dist - min_dist);
	return max_scale - scale;
}

struct local_features
{
	struct crosshair
	{
		enum ch_shape : size_t
		{
			cross,
			square,
			circle,
			gapped_cross,
			gapped_cross_bottom
		};
		bool enabled = false;
		bool filled = true;
		size_t shape = cross;
		double width = 20.0f;
		double height = 20.0f;
		double thickness = 2.0f;
		double gap = 6.0f;

		D2D1_COLOR_F color = { .4f,.8f,.4f,.8f};

		bool use_dynamic_opacity = false;
		double curr_dyn_opacity = 0.5f;
		double fast_opacity = 1.0f;
		double slow_opacity = 0.1f;
		double move_alpha = 0.9f;
		double settle_alpha = 0.01f;

		long mouse_x_delta_threshold = 20;
		long mouse_y_delta_threshold = 10;

		bool use_disable_key = true;
		DiInputManager::DiInput temp_disable_hk = DiInputManager::vM_RIGHTBTN;
		DiInputManager::DiInput toggle_disable_hk = DiInputManager::vKb_F7;


	} g_crosshair;
} g_local_features;





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

	// grp->toggle(&b[0], L"Toggle");
	// grp->slider_double(&f64, -10, 10, L"Slip Speed: %.2lf M/s", [](long i)
	// {
	// });
	// grp->slider_long(&i32, -120, 120, L"Slide %d mph", [](long i)
	// {
	// });
	// grp->modal_selection(&selection,
	// 	L"Aim Bone: %s",
	//                      L"Select Aim Bone",
	// 	{L"Head", L"Neck", L"Chest", L"Pelvis", L"Wiener", L"Extremities"},
	// 	win1);
	// grp->modal_color(&myColor, L"My Color", win1);
	// grp->modal_hotkey(&myHotkey, L"My Hotkey: %s", L"My Hotkey", win1);
	//
	//
	// auto aim_tar = aim->add_tab(L"Target Select");
	// auto aim_adv = aim->add_tab(L"Advanced");
	//
	// cat1->add_section(L"ESP", L"icons/eye_scan.png");
	// cat1->add_section(L"Loot Filter", L"icons/crown.png");
	// cat1->add_section(L"Radar", L"icons/radar.png");


	auto cat2 = win1->add_category(L"CAT2");
	{
		auto misc = cat2->add_section(L"Miscellaneous", L"icons/magic_wand.png");
		auto crosshair_tab = misc->add_tab(L"Crosshair");

		auto basic = crosshair_tab->add_group(L"Basic");
		basic->toggle(&g_local_features.g_crosshair.enabled,L"Enabled");
		basic->modal_selection(&g_local_features.g_crosshair.shape, 
			L"Shape: %s", L"Select Crosshair Shape", 
			{ L"Cross", L"Square", L"Circle", L"4 Segment Cross", L"3 Segment Cross"  }, win1);

		basic->slider_double(&g_local_features.g_crosshair.width, 1, 100, L"Width: %.0fpx");
		basic->slider_double(&g_local_features.g_crosshair.height, 1, 100, L"Height: %.0fpx");
		basic->slider_double(&g_local_features.g_crosshair.thickness, 1, 10, L"Thickness: %.0fpx");
		basic->slider_double(&g_local_features.g_crosshair.gap, 1, 50, L"Gap: %.0fpx");
		basic->modal_color(&g_local_features.g_crosshair.color, L"Color", win1);



		auto hotkeys = crosshair_tab->add_group(L"Hotkeys");
		hotkeys->toggle(&g_local_features.g_crosshair.use_disable_key, L"Enabled");
		hotkeys->modal_hotkey(&g_local_features.g_crosshair.temp_disable_hk, L"Hold to disable: %s", L"Hold-to-disable Key", win1);
		hotkeys->modal_hotkey(&g_local_features.g_crosshair.toggle_disable_hk, L"Toggle: %s", L"Toggle Crosshair Key", win1);


		auto dyn_opacity = crosshair_tab->add_group(L"Dynamic Opacity");
		dyn_opacity->toggle(&g_local_features.g_crosshair.use_dynamic_opacity, L"Enabled");
		dyn_opacity->slider_double(&g_local_features.g_crosshair.fast_opacity, 0.0, 1.00, L"Moving Opacity: %.2f");
		dyn_opacity->slider_double(&g_local_features.g_crosshair.slow_opacity, 0.0, 1.00, L"Static Opacity: %.2f");
		dyn_opacity->slider_double(&g_local_features.g_crosshair.move_alpha, 0.0, 1.00, L"Ramp-Up Alpha: %.3f");
		dyn_opacity->slider_double(&g_local_features.g_crosshair.settle_alpha, 0.0, 1.00, L"Settle Alpha: %.3f");
		dyn_opacity->slider_long(&g_local_features.g_crosshair.mouse_x_delta_threshold, 1, 100, L"Horizontal Move Threshold: %dpx/tick");
		dyn_opacity->slider_long(&g_local_features.g_crosshair.mouse_y_delta_threshold, 1, 100, L"Vertical Move Threshold: %dpx/tick");

	}


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
	if (inputs->IsInputJustReleased(DiInputManager::vKb_F12))
	{
		Renderer::D2DxOverlay::exit = true;
		exit_thread = true;
	}

	if (inputs->IsInputJustReleased(DiInputManager::vKb_INSERT))
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

	renderer->DrawStringC(s2ws((char*) & model::g_menu_settings.some_str), 20.0f, {10, 110}, {1, 1, 1, 1});



	g_enemy_shm.read(g_enemy_array);
	for (const auto& ent : g_enemy_array) {
		if (ent.stale) { break; }
		if (ent.x < 0 || ent.x > width || ent.y < 0 || ent.y > height) { continue; }

		D2D1_COLOR_F ent_col = ESP_COLORS::self;
		switch(ent.team){
			//SELF already assigned
		case model::humanoid::TEAM:
			ent_col = ESP_COLORS::team;
			break;
		case model::humanoid::ENEMY:
			ent_col = ESP_COLORS::enemy;
			break;
		case model::humanoid::TEAM_BOT:
			ent_col = ESP_COLORS::team_bot;
			break;
		case model::humanoid::ENEMY_BOT:
			ent_col = ESP_COLORS::enemy_bot;
			break;
		}





		try
		{
			float thiccness = 2.0f;
			renderer->DrawLineC(ent.bones[model::humanoid::bone::head], ent.bones[model::humanoid::bone::neck], thiccness, ent_col); // Neck
			{
				// Arms
				renderer->DrawLineC(ent.bones[model::humanoid::bone::neck], ent.bones[model::humanoid::bone::l_shoulder], thiccness, ent_col); // Left Upper Arm
				renderer->DrawLineC(ent.bones[model::humanoid::bone::neck], ent.bones[model::humanoid::bone::r_shoulder], thiccness, ent_col); // Right Upper Arm
				renderer->DrawLineC(ent.bones[model::humanoid::bone::l_shoulder], ent.bones[model::humanoid::bone::l_elbow], thiccness, ent_col); // Left Upper Arm
				renderer->DrawLineC(ent.bones[model::humanoid::bone::r_shoulder], ent.bones[model::humanoid::bone::r_elbow], thiccness, ent_col); // Right Upper Arm
				renderer->DrawLineC(ent.bones[model::humanoid::bone::l_elbow], ent.bones[model::humanoid::bone::l_hand], thiccness, ent_col);; // Left Lower Arm
				renderer->DrawLineC(ent.bones[model::humanoid::bone::r_elbow], ent.bones[model::humanoid::bone::r_hand], thiccness, ent_col); // Right Lower Arm
			}
			{
				// Body
				renderer->DrawLineC(ent.bones[model::humanoid::bone::neck], ent.bones[model::humanoid::bone::chest], thiccness, ent_col); // Upper Chest
				renderer->DrawLineC(ent.bones[model::humanoid::bone::chest], ent.bones[model::humanoid::bone::pelvis], thiccness, ent_col); // Torso
			}
			{
				// Legs
				renderer->DrawLineC(ent.bones[model::humanoid::bone::pelvis], ent.bones[model::humanoid::bone::l_hip], thiccness, ent_col); // Left Upper Leg
				renderer->DrawLineC(ent.bones[model::humanoid::bone::pelvis], ent.bones[model::humanoid::bone::r_hip], thiccness, ent_col); // Right Upper Leg
				renderer->DrawLineC(ent.bones[model::humanoid::bone::l_hip], ent.bones[model::humanoid::bone::l_knee], thiccness, ent_col); // Left Upper Leg
				renderer->DrawLineC(ent.bones[model::humanoid::bone::r_hip], ent.bones[model::humanoid::bone::r_knee], thiccness, ent_col); // Right Upper Leg
				renderer->DrawLineC(ent.bones[model::humanoid::bone::l_knee], ent.bones[model::humanoid::bone::l_foot], thiccness, ent_col); // Left Lower Leg
				renderer->DrawLineC(ent.bones[model::humanoid::bone::r_knee], ent.bones[model::humanoid::bone::r_foot], thiccness, ent_col); // Right Lower Leg
			}


			// smaller when further away, larger when closer
			float head_diameter = scale_on_dist(ent.distance, 2.0f, 40.0f, 1.0f, 15.0f);

			renderer->DrawCustomEllipse(
				{ ent.bones[model::humanoid::bone::head].x, ent.bones[model::humanoid::bone::head].y - (head_diameter/2) },
				head_diameter, 
				head_diameter,
				true, 
				{ ent_col.r, ent_col.g, ent_col.b, 0.75f },
				1, 
				ent_col
			);


		}catch(...){}


		// if (ent.isVisible) {
		// 	renderer->DrawCustomEllipse({ ent.x,ent.y }, 5, 5, true, { 1,0,0,1 }, 0, { 0,0,0,0 });
		//
		// }
		// else {
		// 	renderer->DrawCustomEllipse({ ent.x,ent.y }, 5, 5, true, { 0,0,1,1 }, 0, { 0,0,0,0 });
		//
		// }
		renderer->DrawStringCenteredC((wchar_t*)ent.name, 12, { ent.x,ent.y + 15 }, { 0.8f,0.8f,0.8f,1 });

		static wchar_t buff[256];
		if (swprintf(buff, 256, L"%.1fm", ent.distance) < 0)
		{
			ERR(L"BAD swprintf");
		}
		const std::wstring ws(buff);
		
		renderer->DrawStringCenteredC(ws.c_str(), 12, { ent.x,ent.y + 30 }, { 0.8f,0.8f,0.8f,1 });
	}
	
}

// Draw the main GUI and gui components, only modify if needed
// Note: all components (windows, groups, controls) are rendered automatically
void render_gui(UINT32 width, UINT32 height, LPPOINT cur_pos)
{
	h_gui::globals::gui->render(width, height, 0, cur_pos);
}

// Draw directly to the screen ON TOP of the main GUI
void render_direct_post(UINT32 width, UINT32 height, LPPOINT cur_pos, Renderer::D2DxOverlay* renderer, std::shared_ptr<DiInputManager> inputs)
{


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



	{ //crosshair
		if (inputs->IsInputJustReleased(g_local_features.g_crosshair.toggle_disable_hk))
		{
			g_local_features.g_crosshair.enabled = !g_local_features.g_crosshair.enabled;
		}
		if (g_local_features.g_crosshair.enabled && !inputs->IsInputDown(g_local_features.g_crosshair.temp_disable_hk))
		{
			D2D1_COLOR_F final_color = g_local_features.g_crosshair.color;

			if (g_local_features.g_crosshair.use_dynamic_opacity)
			{
				if (abs(inputs->GetMouseDeltaX()) > g_local_features.g_crosshair.mouse_x_delta_threshold || abs(inputs->GetMouseDeltaY()) > g_local_features.g_crosshair.mouse_y_delta_threshold)
				{
					g_local_features.g_crosshair.curr_dyn_opacity = anim::lerp(g_local_features.g_crosshair.curr_dyn_opacity, g_local_features.g_crosshair.fast_opacity, g_local_features.g_crosshair.move_alpha);
				}
				else
				{
					g_local_features.g_crosshair.curr_dyn_opacity = anim::lerp(g_local_features.g_crosshair.curr_dyn_opacity, g_local_features.g_crosshair.slow_opacity, g_local_features.g_crosshair.settle_alpha);
				}
				final_color.a = g_local_features.g_crosshair.curr_dyn_opacity;
			}

			float w_off = g_local_features.g_crosshair.width / 2;
			float h_off = g_local_features.g_crosshair.height / 2;
			D2D1_POINT_2F center = { (float)width / 2 , (float)height / 2 };

			switch (g_local_features.g_crosshair.shape)
			{

			default: // fallthrough to cross
			case local_features::crosshair::ch_shape::cross:
			{
				float half_thick = g_local_features.g_crosshair.thickness / 2;
				renderer->DrawCustomRect({ center.x - w_off,center.y - half_thick,center.x + w_off, center.y + half_thick }, true, false, final_color, {});
				renderer->DrawCustomRect({ center.x - half_thick,center.y - h_off,center.x + half_thick, center.y + h_off }, true, false, final_color, {});
			}break;
			case local_features::crosshair::ch_shape::circle:
			{
				renderer->DrawCustomEllipse(center, g_local_features.g_crosshair.width, g_local_features.g_crosshair.height, true, final_color, 0, {});
			}break;
			case local_features::crosshair::ch_shape::square:
			{
				renderer->DrawCustomRect({ center.x - w_off,center.y - h_off,center.x + w_off,center.y + h_off }, true, 0, final_color, {});
			}break;
			case local_features::crosshair::ch_shape::gapped_cross:
			{
				float half_thick = g_local_features.g_crosshair.thickness / 2;
				float half_gap = g_local_features.g_crosshair.gap / 2;

				renderer->DrawCustomRect({ center.x - w_off,center.y - half_thick,center.x - half_gap, center.y + half_thick }, true, false, final_color, {});
				renderer->DrawCustomRect({ center.x + half_gap,center.y - half_thick,center.x + w_off, center.y + half_thick }, true, false, final_color, {});

				renderer->DrawCustomRect({ center.x - half_thick,center.y - h_off,center.x + half_thick, center.y - half_gap }, true, false, final_color, {});
				renderer->DrawCustomRect({ center.x - half_thick,center.y + half_gap,center.x + half_thick, center.y + h_off }, true, false, final_color, {});

			}break;
			case local_features::crosshair::ch_shape::gapped_cross_bottom:
			{
				float half_thick = g_local_features.g_crosshair.thickness / 2;
				float half_gap = g_local_features.g_crosshair.gap / 2;

				renderer->DrawCustomRect({ center.x - w_off,center.y - half_thick,center.x - half_gap, center.y + half_thick }, true, false, final_color, {});
				renderer->DrawCustomRect({ center.x + half_gap,center.y - half_thick,center.x + w_off, center.y + half_thick }, true, false, final_color, {});

				//renderer->DrawCustomRect({ center.x - half_thick,center.y - h_off,center.x + half_thick, center.y - half_gap }, true, false, final_color, {});
				renderer->DrawCustomRect({ center.x - half_thick,center.y + half_gap,center.x + half_thick, center.y + h_off }, true, false, final_color, {});

			}break;
			}
		}
	}//end crosshair


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
	render_direct_post(width, height, cur_pos, renderer, inputs);
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
