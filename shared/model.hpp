#pragma once

namespace model
{
	constexpr size_t ENEMY_ARRAY_SIZE = 256;
	typedef D2D1_POINT_2F Vector2;

	// struct Vector2
	// {
	// 	float x;
	// 	float y;
	// };


	struct humanoid
	{
		bool stale = true;
		float x = 0;
		float y = 0;
		uint16_t name[128] = {};
		float distance = 0;
		bool isVisible = false;
		uint8_t team = 0;
		Vector2 bones[16];


		enum bone : uint8_t
		{
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
	};


	// inline void draw_skeleton(const humanoid& ent)
	// {
	// 	line(ent.bones[humanoid::bone::head], ent.bones[humanoid::bone::neck_base]); // Neck
	// 	{
	// 		// Arms
	// 		line(ent.bones[humanoid::bone::neck_base], ent.bones[humanoid::bone::l_elbow]); // Left Upper Arm
	// 		line(ent.bones[humanoid::bone::neck_base], ent.bones[humanoid::bone::r_elbow]); // Right Upper Arm
	// 		line(ent.bones[humanoid::bone::l_elbow], ent.bones[humanoid::bone::l_hand]); // Left Lower Arm
	// 		line(ent.bones[humanoid::bone::r_elbow], ent.bones[humanoid::bone::r_hand]); // Right Lower Arm
	// 	}
	// 	{
	// 		// Body
	// 		line(ent.bones[humanoid::bone::neck_base], ent.bones[humanoid::bone::chest]); // Upper Chest
	// 		line(ent.bones[humanoid::bone::chest], ent.bones[humanoid::bone::pelvis]); // Torso
	// 	}
	// 	{
	// 		// Legs
	// 		line(ent.bones[humanoid::bone::pelvis], ent.bones[humanoid::bone::l_knee]); // Left Upper Leg
	// 		line(ent.bones[humanoid::bone::pelvis], ent.bones[humanoid::bone::r_knee]); // Right Upper Leg
	// 		line(ent.bones[humanoid::bone::l_knee], ent.bones[humanoid::bone::l_foot]); // Left Lower Leg
	// 		line(ent.bones[humanoid::bone::r_knee], ent.bones[humanoid::bone::r_foot]); // Right Lower Leg
	// 	}
	// }




	// struct enemy
	// {
	// 	bool stale = true;
	// 	float x = 0;
	// 	float y = 0;
	// 	uint16_t name[128] = {};
	// 	float distance = 0;
	// 	bool isVisible = false;
	// 	uint8_t team = 0;
	// };

	struct menu_settings
	{
		uint8_t some_str[128] = "";
	} inline g_menu_settings;

	struct client_settings
	{
		struct
		{
			bool enabled;
		} esp;

		struct
		{
			bool enabled;
		} aimbot;
	} inline g_client_settings;
}
