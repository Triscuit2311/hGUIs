#pragma once
namespace h_style
{
	namespace structural
	{
		namespace base
		{
			constexpr float block_height = 20.0f;
			constexpr float margin = 6.0f;
			constexpr float pad = 2.0f;
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
			constexpr float top_bar_height = space * 2.0f;
			constexpr float side_bar_width = control_width;
			constexpr float side_bar_bottom_widget_height = space * 3.0f;

			constexpr float tab_select_height = space;
			constexpr float control_groups_width = (control_width * 2.0f) + (space * 5.0f);
			constexpr float control_groups_height = (control_width * 2.0f) + (space * 3.0f);

			constexpr float width = side_bar_width + control_groups_width;
			constexpr float height = top_bar_height + tab_select_height + control_groups_height;

		}



	}

	namespace theme
	{
		constexpr float border_stroke = 1.0f;
		constexpr float border_radius = 8.0f;

		namespace text
		{
			constexpr float font_size = 14.0f;

			inline float get_text_width(const size_t len)
			{
				// weak implementation
				// intend on using DirectWrite API with text metrics
				// but the api is terrible
				// TODO: parse the font glyphs and calc
				return static_cast<float>(len) * 0.65f * (font_size);
			}

			//TODO: center in area

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
			namespace base
			{
				constexpr D2D1_COLOR_F bg {rgb_to_color_f(28,31,31,0.2f)};
				constexpr D2D1_COLOR_F fg {rgb_to_color_f(179,180,181,1.0f)};
				constexpr D2D1_COLOR_F fg_hi{1,1,1,1};


				constexpr D2D1_COLOR_F accent_a = rgb_to_color_f(255, 201, 74, 1.0f);
				constexpr D2D1_COLOR_F accent_b = rgb_to_color_f(74, 255, 110,1.0f);

			}

			namespace window
			{
				constexpr D2D1_COLOR_F border{0,0,0, 1.0f};
				constexpr D2D1_COLOR_F border_hovered{ 0.1f, 0.1f, 0.1f, 1.0f };
			}

			namespace group
			{
				constexpr D2D1_COLOR_F border{0.4f, 0.4f, 0.4f, 0.7f};
				constexpr D2D1_COLOR_F border_hovered{0.9f, 0.9f, 0.9f, 1.0f};
			}

			namespace control
			{
				constexpr D2D1_COLOR_F toggle{0.6f, 0.6f, 0.6f, 0.7f};
				constexpr D2D1_COLOR_F toggle_hovered{0.6f, 0.6f, 0.6f, 0.9f};
				constexpr D2D1_COLOR_F toggle_marker{0.2f, 0.2f, 0.2f, 0.7f};

				constexpr D2D1_COLOR_F slider{0.6f, 0.6f, 0.6f, 0.7f};
				constexpr D2D1_COLOR_F slider_hovered{0.6f, 0.6f, 0.6f, 0.9f};
				constexpr D2D1_COLOR_F slider_handle{0.6f, 0.6f, 0.6f, 1.0f};


				constexpr D2D1_COLOR_F button{ 0.6f, 0.6f, 0.6f, 1.0f };
				constexpr D2D1_COLOR_F button_stroke{ 0.3f, 0.3f, 0.4f, 1.0f };
				constexpr D2D1_COLOR_F button_hovered{ 0.7f, 0.7f, 0.7f, 1.0f };
				constexpr D2D1_COLOR_F button_pressed{ 0.8f, 1.0f, 0.8f, 1.0f };

			}



		}

	}

}
