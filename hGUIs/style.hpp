#pragma once
namespace h_style
{
	namespace structural
	{
		namespace base
		{
			constexpr float block_height = 25.0f;
			constexpr float margin = 5.0f;
			constexpr float pad = 3.0f;
		}

		namespace factors
		{
			constexpr float control_width_factor = 12.0f;
			constexpr float space_factor = 5.0f;
		}

		constexpr float control_width = base::block_height * factors::control_width_factor;
		constexpr float space = base::margin * factors::space_factor;


		namespace window
		{
			constexpr float top_bar_height = space;
			constexpr float side_bar_width = control_width;
			constexpr float side_bar_bottom_widget_height = space * 3.0f;

			constexpr float tab_select_height = space * 1.5f;
			constexpr float control_groups_width = (control_width * 2.0f) + (space * 5.0f);
			constexpr float control_groups_height = (control_width * 2.0f) + (space * 3.0f);

			constexpr float width = side_bar_width + control_groups_width;
			constexpr float height = top_bar_height + tab_select_height + control_groups_height;

		}

		namespace tab
		{
			constexpr float tab_grab_height = window::tab_select_height * 0.75f;
		}

		namespace control
		{
			constexpr float toggle_marker_width = base::block_height*2;
		}



	}

	namespace theme
	{
		constexpr float border_stroke = 1.0f;
		constexpr float border_radius = 8.0f;

		namespace text
		{
			constexpr float font_size_s = 12.0f;
			constexpr float font_size_m = 16.0f;
			constexpr float font_size_L = 20.0f;

		}


		namespace colors
		{

			namespace
			{
				constexpr D2D1_COLOR_F rgb_to_color_f(int r, int g, int b, float a)
				{
					return {
						float(r) / 255.0f,
						float(g) / 255.0f,
						float(b) / 255.0f,
						a,
					};
				}
			}

			namespace palette
			{
				constexpr D2D1_COLOR_F med_gray{ rgb_to_color_f(45,45,45,1.0f) };
				constexpr D2D1_COLOR_F med_gray2{ rgb_to_color_f(51,51,51,1.0f) };
				constexpr D2D1_COLOR_F med_gray3{ rgb_to_color_f(80,80,80,1.0f) };
				constexpr D2D1_COLOR_F med_gray4{ rgb_to_color_f(100,100,100,1.0f) };
				constexpr D2D1_COLOR_F dark_gray{ rgb_to_color_f(28,31,31,1.0f) };
				constexpr D2D1_COLOR_F off_white{ rgb_to_color_f(179,180,181,1.0f) };

			}


			namespace base
			{
				constexpr D2D1_COLOR_F bg {palette::dark_gray};
				constexpr D2D1_COLOR_F fg {palette::off_white};
				constexpr D2D1_COLOR_F fg_hi{1,1,1,1};


				constexpr D2D1_COLOR_F accent_a = rgb_to_color_f(255, 201, 74, 1.0f);
				constexpr D2D1_COLOR_F accent_b = rgb_to_color_f(74, 255, 110,1.0f);

			}

			namespace window
			{
				constexpr D2D1_COLOR_F separator{palette::med_gray2};
				constexpr D2D1_COLOR_F border{palette::med_gray};

			}

			namespace section
			{
				constexpr D2D1_COLOR_F selected_bg = palette::med_gray;
				constexpr D2D1_COLOR_F selected_stroke = palette::med_gray2;
				constexpr D2D1_COLOR_F text_unselected = palette::med_gray3;
				constexpr D2D1_COLOR_F text_unselected_hov = palette::med_gray4;
				constexpr D2D1_COLOR_F text_selected = base::fg;
			}

			namespace group
			{
				constexpr D2D1_COLOR_F border{0.4f, 0.4f, 0.4f, 0.7f};
				constexpr D2D1_COLOR_F border_hovered{0.9f, 0.9f, 0.9f, 1.0f};
			}
			 

			namespace control
			{

				constexpr D2D1_COLOR_F toggle_bg_off{palette::med_gray2 };
				constexpr D2D1_COLOR_F toggle_bg_on{palette::med_gray4 };

				constexpr D2D1_COLOR_F toggle_marker_off{base::fg};
				constexpr D2D1_COLOR_F toggle_marker_on{base::fg_hi};



				constexpr D2D1_COLOR_F slider{0.6f, 0.6f, 0.6f, 0.7f};
				constexpr D2D1_COLOR_F slider_hovered{0.6f, 0.6f, 0.6f, 0.9f};
				constexpr D2D1_COLOR_F slider_handle{0.6f, 0.6f, 0.6f, 1.0f};


				constexpr D2D1_COLOR_F button{ 0.6f, 0.6f, 0.6f, 1.0f };
				constexpr D2D1_COLOR_F button_stroke{ 0.3f, 0.3f, 0.4f, 1.0f };
				constexpr D2D1_COLOR_F button_hovered{ 0.7f, 0.7f, 0.7f, 1.0f };
				constexpr D2D1_COLOR_F button_pressed{ 0.8f, 1.0f, 0.8f, 1.0f };

			}

			namespace tab
			{
				constexpr D2D1_COLOR_F select_face = palette::med_gray;
				constexpr D2D1_COLOR_F text_unselected = palette::med_gray3;
				constexpr D2D1_COLOR_F text_unselected_hov = palette::med_gray4;

				constexpr D2D1_COLOR_F text_selected = base::fg;

				constexpr D2D1_COLOR_F tab_page = palette::med_gray;
				constexpr D2D1_COLOR_F select_bg = palette::dark_gray;

			}


			namespace group
			{
				constexpr D2D1_COLOR_F bg = palette::dark_gray;
				constexpr D2D1_COLOR_F bg_stroke = palette::med_gray2;
			}



		}

	}

}
