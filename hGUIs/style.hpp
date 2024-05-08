#pragma once
namespace h_gui_style
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
			constexpr float control_width_factor = 10.0f;
		}

		constexpr float control_width = base::block_height * factors::control_width_factor;
		constexpr float group_width = control_width + (2 * base::margin);
		constexpr float window_width = group_width + (2 * base::margin);
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
			namespace base
			{
				constexpr D2D1_COLOR_F bg{0.2f, 0.2f, 0.2f, 0.8f};
				constexpr D2D1_COLOR_F fg{0.8f, 0.8f, 0.8f, 0.9f};
			}

			namespace window
			{
				constexpr D2D1_COLOR_F border{0.9f, 0.9f, 0.9f, 0.8f};
				constexpr D2D1_COLOR_F border_hovered{0.9f, 0.9f, 0.9f, 1.0f};
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
