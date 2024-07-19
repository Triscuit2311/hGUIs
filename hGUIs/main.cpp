#include "pch.h"

#include "animations.hpp"
#include "D2xOverlay.h"
#include "DiInputManager.h"
#include "hGUI.h"
#include "InputDefines.h"
#include "WindowsUtils.h"
#include "../shared/shared_mem.hpp"

#include "../shared/model.hpp"


_internal_shm::shared_memory_single<model::menu_settings> menu_shm(L"SHM_MENU_SETTINGS");
_internal_shm::shared_memory_single<model::client_settings> client_shm(L"SHM_CLIENT_SETTINGS");

shm_arr<model::humanoid, model::PLAYER_ARRAY_SZ> g_player_shm(L"PLAYER_SHM");
shm_arr<model::marker, model::MARKER_ARRAY_SZ> g_marker_shm(L"MARKERS_SHM");

Renderer::D2DBitmapID splash_img;

bool use_splash = false;
bool exit_thread = false;


namespace VISUALS_COLORS
{
	inline static D2D1_COLOR_F self{.1f, .1f, .1f, 1}; // dark gray


	inline static D2D1_COLOR_F enemy{0.9f, 0.9f, 0.0f, 1};
	inline static D2D1_COLOR_F enemy_vis{1.0f, 0.2f, 0.0f, 1};

	inline static D2D1_COLOR_F enemy_bot{.9f, .5f, 0.0f, 1};
	inline static D2D1_COLOR_F enemy_bot_vis{1.0f, 0.0f, .5f, 1};


	inline static D2D1_COLOR_F team{0.2f, 0.6f, 0.3f, 1};
	inline static D2D1_COLOR_F team_vis{0.0f, 1.0f, 0.5f, 1};

	inline static D2D1_COLOR_F team_bot{.2f, .4f, .6f, 1};
	inline static D2D1_COLOR_F team_bot_vis{0.f, 0.75f, 1.f, 1};


	inline static D2D1_COLOR_F target{1, 1, .1f, 1};
	inline static D2D1_COLOR_F active_target{1, 1, 1, 1};
}


float scale_on_dist(float distance, float min_dist, float max_dist, float min_scale, float max_scale)
{
	if (distance < min_dist) { return max_scale; }
	if (distance > max_dist) { return min_scale; }

	distance = std::clamp(distance, min_dist, max_dist);
	float scale = min_scale + (distance - min_dist) * (max_scale - min_scale) / (max_dist - min_dist);

	return std::clamp(max_scale - scale, min_scale, max_dist);
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

		D2D1_COLOR_F color = {.4f, .8f, .4f, .8f};

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


std::wstring s2ws(char* cStr)
{
	std::wstring wStr;
	wStr.reserve(strlen(cStr) + 1);
	for (int i = 0; i < strlen(cStr); i++)
	{
		wStr += wchar_t(cStr[i]);
	}
	wStr += L'\0'; // Add the null-terminator
	return wStr;
}

// Runs once at start-time, initializes all the windows, groups and controls for the GUI
void setup_gui(const std::shared_ptr<h_gui::workspace>& ws)
{
	const auto win1 = ws->add_window(L"hGUI Prototype v0.63b", {500, 200});

	auto cat = win1->add_category(L"CAT1");
	{
		auto aim = cat->add_section(L"Aimbot", L"icons/target.png");

		auto aim_gen = aim->add_tab(L"General");
		auto grp = aim_gen->add_group(L"Global");
		grp->toggle(&model::g_client_settings.aimbot.enabled, L"Enabled");
	}

	cat = win1->add_category(L"CAT2");
	{
		auto esp = cat->add_section(L"ESP", L"icons/eye_scan.png");
		auto aim_gen = esp->add_tab(L"General");
		auto grp = aim_gen->add_group(L"Global");
		grp->toggle(&model::g_client_settings.esp.enabled, L"Enabled");
	}

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
	//cat = win1->add_category(L"CAT2");
	//cat1->add_section(L"ESP", L"icons/eye_scan.png");


	// cat1->add_section(L"Loot Filter", L"icons/crown.png");
	// cat1->add_section(L"Radar", L"icons/radar.png");


	auto cat2 = win1->add_category(L"CAT2");
	{
		auto misc = cat2->add_section(L"Miscellaneous", L"icons/magic_wand.png");

		auto memory_tab = misc->add_tab(L"Memory Hacks");
		auto weapon = memory_tab->add_group(L"Weapon");
		weapon->toggle(&model::g_client_settings.memory.recoil_control, L"Recoil Control");
		weapon->slider_double(&model::g_client_settings.memory.recoil_percent, 0.00, 1.0, L"Recoil Amount: %.1f");
		weapon->toggle(&model::g_client_settings.memory.spread_control, L"Spread Control");
		weapon->slider_double(&model::g_client_settings.memory.spread_percent, 0.00, 1.0, L"Spread Amount: %.1f");
		weapon->toggle(&model::g_client_settings.memory.no_shake, L"No Shake");
		//weapon->toggle(&model::g_client_settings.memory.full_auto, L"Full Auto");


		auto player = memory_tab->add_group(L"Player");
		player->toggle(&model::g_client_settings.memory.instant_lean, L"Instant Lean");
		player->toggle(&model::g_client_settings.memory.super_lean, L"Super Lean");


		auto crosshair_tab = misc->add_tab(L"Crosshair");

		auto basic = crosshair_tab->add_group(L"Basic");
		basic->toggle(&g_local_features.g_crosshair.enabled, L"Enabled");
		basic->modal_selection(&g_local_features.g_crosshair.shape,
		                       L"Shape: %s", L"Select Crosshair Shape",
		                       {L"Cross", L"Square", L"Circle", L"4 Segment Cross", L"3 Segment Cross"}, win1);

		basic->slider_double(&g_local_features.g_crosshair.width, 1, 100, L"Width: %.0fpx");
		basic->slider_double(&g_local_features.g_crosshair.height, 1, 100, L"Height: %.0fpx");
		basic->slider_double(&g_local_features.g_crosshair.thickness, 1, 10, L"Thickness: %.0fpx");
		basic->slider_double(&g_local_features.g_crosshair.gap, 1, 50, L"Gap: %.0fpx");
		basic->modal_color(&g_local_features.g_crosshair.color, L"Color", win1);


		auto hotkeys = crosshair_tab->add_group(L"Hotkeys");
		hotkeys->toggle(&g_local_features.g_crosshair.use_disable_key, L"Enabled");
		hotkeys->modal_hotkey(&g_local_features.g_crosshair.temp_disable_hk, L"Hold to disable: %s",
		                      L"Hold-to-disable Key", win1);
		hotkeys->modal_hotkey(&g_local_features.g_crosshair.toggle_disable_hk, L"Toggle: %s", L"Toggle Crosshair Key",
		                      win1);


		auto dyn_opacity = crosshair_tab->add_group(L"Dynamic Opacity");
		dyn_opacity->toggle(&g_local_features.g_crosshair.use_dynamic_opacity, L"Enabled");
		dyn_opacity->slider_double(&g_local_features.g_crosshair.fast_opacity, 0.0, 1.00, L"Moving Opacity: %.2f");
		dyn_opacity->slider_double(&g_local_features.g_crosshair.slow_opacity, 0.0, 1.00, L"Static Opacity: %.2f");
		dyn_opacity->slider_double(&g_local_features.g_crosshair.move_alpha, 0.0, 1.00, L"Ramp-Up Alpha: %.3f");
		dyn_opacity->slider_double(&g_local_features.g_crosshair.settle_alpha, 0.0, 1.00, L"Settle Alpha: %.3f");
		dyn_opacity->slider_long(&g_local_features.g_crosshair.mouse_x_delta_threshold, 1, 100,
		                         L"Horizontal Move Threshold: %dpx/tick");
		dyn_opacity->slider_long(&g_local_features.g_crosshair.mouse_y_delta_threshold, 1, 100,
		                         L"Vertical Move Threshold: %dpx/tick");
	}


	cat2->add_section(L"Experimental", L"icons/test_tube.png");

	auto cat3 = win1->add_category(L"CAT3");
	{
		auto col_sec = cat3->add_section(L"Colors", L"icons/pallete.png");
		auto vistab = col_sec->add_tab(L"Visuals");
		auto g1 = vistab->add_group(L"Player ESP");
		g1->modal_color(&VISUALS_COLORS::enemy, L"Enemy Players", win1);
		g1->modal_color(&VISUALS_COLORS::enemy_bot, L"Enemy Bots", win1);
		g1->modal_color(&VISUALS_COLORS::team, L"Team Players", win1);
		g1->modal_color(&VISUALS_COLORS::team_bot, L"Team Bots", win1);

		auto g2 = vistab->add_group(L"Aimbot");
		g2->modal_color(&VISUALS_COLORS::target, L"Target Dot", win1);
		g2->modal_color(&VISUALS_COLORS::active_target, L"Active Target Dot", win1);
	}


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

	//if (inputs->IsInputJustReleased(DiInputManager::vKb_INSERT))
	if (GetAsyncKeyState(VK_INSERT) & 1)
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

	renderer->DrawStringC(s2ws((char*)&model::g_menu_settings.some_str), 20.0f, {10, 110}, {1, 1, 1, 1});


	g_marker_shm.read();
	for (auto& m : g_marker_shm)
	{
		if (m.stale) { continue; }
		D2D1_COLOR_F col = {1, 1, 1, 1};
		if (m.visible)
		{
			col = {0, 1, 0, 1};
		}
		renderer->DrawCustomEllipse(
			{m.spos.x, m.spos.y - 2},
			2,
			2,
			true,
			col,
			0,
			{1, 1, 1, 0.8f}
		);
	}

	g_player_shm.read();
	for (const auto& ent : g_player_shm)
	{
		if (ent.stale) { break; }
		if (ent.x < 0 || ent.x > width || ent.y < 0 || ent.y > height) { continue; }

		D2D1_COLOR_F ent_col = VISUALS_COLORS::self;
		D2D1_COLOR_F ent_col_vis = VISUALS_COLORS::self;
		switch (ent.team)
		{
		//SELF already assigned
		case model::humanoid::TEAM:
			ent_col = VISUALS_COLORS::team;
			ent_col_vis = VISUALS_COLORS::team_vis;
			break;
		case model::humanoid::ENEMY:
			ent_col = VISUALS_COLORS::enemy;
			ent_col_vis = VISUALS_COLORS::enemy_vis;
			break;
		case model::humanoid::TEAM_BOT:
			ent_col = VISUALS_COLORS::team_bot;
			ent_col_vis = VISUALS_COLORS::team_bot_vis;
			break;
		case model::humanoid::ENEMY_BOT:
			ent_col = VISUALS_COLORS::enemy_bot;
			ent_col_vis = VISUALS_COLORS::enemy_bot_vis;
			break;
		}

		try
		{
			float thiccness = ent.distance < 15 ? 5.0f : ent.distance < 25 ? 3.0f : 2.0f;

			auto line_wrapper = [&](const model::humanoid::bone a, const model::humanoid::bone b)
			{
				renderer->DrawLineC(ent.bones[a], ent.bones[b], thiccness,
				                    (ent.bones_vis[a] && ent.bones_vis[b]) ? ent_col_vis : ent_col
				);
			};


			line_wrapper(model::humanoid::bone::head, model::humanoid::bone::neck);
			line_wrapper(model::humanoid::bone::neck, model::humanoid::bone::l_shoulder); // // Left Upper Arm
			line_wrapper(model::humanoid::bone::neck, model::humanoid::bone::r_shoulder); // // Right Upper Arm
			line_wrapper(model::humanoid::bone::l_shoulder, model::humanoid::bone::l_elbow); // // Left Upper Arm
			line_wrapper(model::humanoid::bone::r_shoulder, model::humanoid::bone::r_elbow); // // Right Upper Arm
			line_wrapper(model::humanoid::bone::l_elbow, model::humanoid::bone::l_hand); //; // Left Lower Arm
			line_wrapper(model::humanoid::bone::r_elbow, model::humanoid::bone::r_hand); // // Right Lower Arm
			line_wrapper(model::humanoid::bone::neck, model::humanoid::bone::chest); // // Upper Chest
			line_wrapper(model::humanoid::bone::chest, model::humanoid::bone::pelvis); // // Torso
			line_wrapper(model::humanoid::bone::pelvis, model::humanoid::bone::l_hip); // // Left Upper Leg
			line_wrapper(model::humanoid::bone::pelvis, model::humanoid::bone::r_hip); // // Right Upper Leg
			line_wrapper(model::humanoid::bone::l_hip, model::humanoid::bone::l_knee); // // Left Upper Leg
			line_wrapper(model::humanoid::bone::r_hip, model::humanoid::bone::r_knee); // // Right Upper Leg

			renderer->DrawLineC(ent.bones[model::humanoid::bone::l_knee], ent.bones[model::humanoid::bone::l_foot],
			                    thiccness,
			                    (ent.bones_vis[model::humanoid::bone::l_knee]) ? ent_col_vis : ent_col
			);
			renderer->DrawLineC(ent.bones[model::humanoid::bone::r_knee], ent.bones[model::humanoid::bone::r_foot],
			                    thiccness,
			                    (ent.bones_vis[model::humanoid::bone::r_knee]) ? ent_col_vis : ent_col
			);


			// todo: fix this, make it less shitty
			// TODO: make it on the aim point dummy
			// if (ent.is_target)
			// {
			// 	ent_col = ent.is_target_active ? VISUALS_COLORS::active_target : VISUALS_COLORS::target;
			// }

			// did better
			auto head_col = (ent.bones_vis[model::humanoid::bone::head]) ? ent_col_vis : ent_col;
			float head_diameter = ent.bones[model::humanoid::bone::top_of_head].y - ent.bones[model::humanoid::bone::head].y;
			renderer->DrawCustomEllipse(
				{
					ent.bones[model::humanoid::bone::head].x,
					ent.bones[model::humanoid::bone::head].y - (head_diameter / 2)
				},
				head_diameter / 2,
				head_diameter / 2,
				true,
				{head_col.r, head_col.g, head_col.b, 0.75f},
				1,
				head_col
			);
		}
		catch (...)
		{
		}


		const float fontsize = 13;

		renderer->DrawStringCenteredC((wchar_t*)ent.name, fontsize, {ent.x + 1, ent.y + 15 + 1}, {0.f, 0.0f, 0.0f, 1});
		renderer->DrawStringCenteredC((wchar_t*)ent.name, fontsize, {ent.x - 1, ent.y + 15 - 1}, {0.f, 0.0f, 0.0f, 1});
		renderer->DrawStringCenteredC((wchar_t*)ent.name, fontsize, {ent.x + 1, ent.y + 15 - 1}, {0.f, 0.0f, 0.0f, 1});
		renderer->DrawStringCenteredC((wchar_t*)ent.name, fontsize, {ent.x - 1, ent.y + 15 + 1}, {0.f, 0.0f, 0.0f, 1});

		renderer->DrawStringCenteredC((wchar_t*)ent.name, fontsize, {ent.x, ent.y + 15}, {0.8f, 0.8f, 0.8f, 1});

		static wchar_t buff[256];
		if (swprintf(buff, 256, L"%.1fm", ent.distance) < 0)
		{
			ERR(L"BAD swprintf");
		}
		const std::wstring ws(buff);

		renderer->DrawStringCenteredC(ws.c_str(), fontsize, {ent.x + 1, ent.y + 30 + 1}, {0.0f, 0.0f, 0.0f, 1});
		renderer->DrawStringCenteredC(ws.c_str(), fontsize, {ent.x - 1, ent.y + 30 - 1}, {0.0f, 0.0f, 0.0f, 1});
		renderer->DrawStringCenteredC(ws.c_str(), fontsize, {ent.x + 1, ent.y + 30 - 1}, {0.0f, 0.0f, 0.0f, 1});
		renderer->DrawStringCenteredC(ws.c_str(), fontsize, {ent.x - 1, ent.y + 30 + 1}, {0.0f, 0.0f, 0.0f, 1});
		renderer->DrawStringCenteredC(ws.c_str(), fontsize, {ent.x, ent.y + 30}, {0.8f, 0.8f, 0.8f, 1});
	}
}

// Draw the main GUI and gui components, only modify if needed
// Note: all components (windows, groups, controls) are rendered automatically
void render_gui(UINT32 width, UINT32 height, LPPOINT cur_pos)
{
	h_gui::globals::gui->render(width, height, 0, cur_pos);
}

// Draw directly to the screen ON TOP of the main GUI
void render_direct_post(UINT32 width, UINT32 height, LPPOINT cur_pos, Renderer::D2DxOverlay* renderer,
                        std::shared_ptr<DiInputManager> inputs)
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


	{
		//crosshair
		if (inputs->IsInputJustReleased(g_local_features.g_crosshair.toggle_disable_hk))
		{
			g_local_features.g_crosshair.enabled = !g_local_features.g_crosshair.enabled;
		}
		if (g_local_features.g_crosshair.enabled && !inputs->IsInputDown(g_local_features.g_crosshair.temp_disable_hk))
		{
			D2D1_COLOR_F final_color = g_local_features.g_crosshair.color;

			if (g_local_features.g_crosshair.use_dynamic_opacity)
			{
				if (abs(inputs->GetMouseDeltaX()) > g_local_features.g_crosshair.mouse_x_delta_threshold ||
					abs(inputs->GetMouseDeltaY()) > g_local_features.g_crosshair.mouse_y_delta_threshold)
				{
					g_local_features.g_crosshair.curr_dyn_opacity = anim::lerp(
						g_local_features.g_crosshair.curr_dyn_opacity, g_local_features.g_crosshair.fast_opacity,
						g_local_features.g_crosshair.move_alpha);
				}
				else
				{
					g_local_features.g_crosshair.curr_dyn_opacity = anim::lerp(
						g_local_features.g_crosshair.curr_dyn_opacity, g_local_features.g_crosshair.slow_opacity,
						g_local_features.g_crosshair.settle_alpha);
				}
				final_color.a = g_local_features.g_crosshair.curr_dyn_opacity;
			}

			float w_off = g_local_features.g_crosshair.width / 2;
			float h_off = g_local_features.g_crosshair.height / 2;
			D2D1_POINT_2F center = {(float)width / 2, (float)height / 2};

			switch (g_local_features.g_crosshair.shape)
			{
			default: // fallthrough to cross
			case local_features::crosshair::ch_shape::cross:
				{
					float half_thick = g_local_features.g_crosshair.thickness / 2;
					renderer->DrawCustomRect({
						                         center.x - w_off, center.y - half_thick, center.x + w_off,
						                         center.y + half_thick
					                         }, true, false, final_color, {});
					renderer->DrawCustomRect({
						                         center.x - half_thick, center.y - h_off, center.x + half_thick,
						                         center.y + h_off
					                         }, true, false, final_color, {});
				}
				break;
			case local_features::crosshair::ch_shape::circle:
				{
					renderer->DrawCustomEllipse(center, g_local_features.g_crosshair.width,
					                            g_local_features.g_crosshair.height, true, final_color, 0, {});
				}
				break;
			case local_features::crosshair::ch_shape::square:
				{
					renderer->DrawCustomRect({center.x - w_off, center.y - h_off, center.x + w_off, center.y + h_off},
					                         true, 0, final_color, {});
				}
				break;
			case local_features::crosshair::ch_shape::gapped_cross:
				{
					float half_thick = g_local_features.g_crosshair.thickness / 2;
					float half_gap = g_local_features.g_crosshair.gap / 2;

					renderer->DrawCustomRect({
						                         center.x - w_off, center.y - half_thick, center.x - half_gap,
						                         center.y + half_thick
					                         }, true, false, final_color, {});
					renderer->DrawCustomRect({
						                         center.x + half_gap, center.y - half_thick, center.x + w_off,
						                         center.y + half_thick
					                         }, true, false, final_color, {});

					renderer->DrawCustomRect({
						                         center.x - half_thick, center.y - h_off, center.x + half_thick,
						                         center.y - half_gap
					                         }, true, false, final_color, {});
					renderer->DrawCustomRect({
						                         center.x - half_thick, center.y + half_gap, center.x + half_thick,
						                         center.y + h_off
					                         }, true, false, final_color, {});
				}
				break;
			case local_features::crosshair::ch_shape::gapped_cross_bottom:
				{
					float half_thick = g_local_features.g_crosshair.thickness / 2;
					float half_gap = g_local_features.g_crosshair.gap / 2;

					renderer->DrawCustomRect({
						                         center.x - w_off, center.y - half_thick, center.x - half_gap,
						                         center.y + half_thick
					                         }, true, false, final_color, {});
					renderer->DrawCustomRect({
						                         center.x + half_gap, center.y - half_thick, center.x + w_off,
						                         center.y + half_thick
					                         }, true, false, final_color, {});

					//renderer->DrawCustomRect({ center.x - half_thick,center.y - h_off,center.x + half_thick, center.y - half_gap }, true, false, final_color, {});
					renderer->DrawCustomRect({
						                         center.x - half_thick, center.y + half_gap, center.x + half_thick,
						                         center.y + h_off
					                         }, true, false, final_color, {});
				}
				break;
			}
		}
	} //end crosshair
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
