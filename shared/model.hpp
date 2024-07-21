#pragma once

namespace model
{
	typedef D2D1_POINT_2F Vector2;

	constexpr size_t PLAYER_ARRAY_SZ = 64;
	constexpr size_t MARKER_ARRAY_SZ = 128;

	struct marker
	{
		bool stale = true;
		Vector2 spos = {-1,-1};
		uint16_t name[128] = {};
		bool visible = false;
	};


	struct humanoid
	{
		enum bone : uint8_t
		{
			top_of_head,
			head,
			neck,
			l_shoulder,
			r_shoulder,
			l_elbow,
			r_elbow,
			l_hand,
			r_hand,
			chest,
			pelvis,
			l_hip,
			r_hip,
			l_knee,
			r_knee,
			r_foot,
			l_foot,
		};

		enum team_type : uint8_t
		{
			SELF,
			TEAM,
			ENEMY,
			TEAM_BOT,
			ENEMY_BOT,
		};

		bool stale = true;
		float x = 0;
		float y = 0;
		uint16_t name[128] = {};
		float distance = 0;
		bool isVisible = false;
		team_type team = SELF;
		Vector2 bones[17];
		bool bones_vis[17] = { false };
		bool is_target = false;
		bool is_target_active = false;
	};



	struct menu_settings
	{
		uint8_t status[128] = "";
		bool is_in_optic = false;
		marker aim_target;
	} inline g_menu_settings;

	struct client_settings
	{
		struct
		{
			bool enabled = true;
			bool skip_bone_vis = false;
		} esp;

		struct
		{
			bool enabled = true;

			bool stick_to_target = true;
			bool switch_target_without_release = false;
			bool silent_aim = false;

			bool use_smoothing = false;
			double smooth_alpha = 0.1f;

			double fov = 0.075f; //%of screen space
			double optic_fov = 0.15f; //%of screen space

			int aim_key_vKey = VK_XBUTTON1;

			bool smart_bone = false;
			humanoid::bone target_bone = humanoid::bone::head;

		} aimbot;

		struct
		{
			bool recoil_control = true;
			double recoil_percent = 0.0f;
			bool spread_control = true;
			double spread_percent = 0.0f;
			bool no_shake = true;
			bool instant_lean = false;
			bool super_lean = false;
		} memory;

	} inline g_client_settings;
}
