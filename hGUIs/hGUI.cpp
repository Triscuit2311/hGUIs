#include "pch.h"
#include "hGUI.h"

#include "animations.hpp"
#include "style.hpp"
#include "logging.h"
#include "WindowsUtils.h"

// controls
namespace h_gui::controls
{
	label::label(std::wstring label_text) : control()
	{
		text = std::move(label_text);
		size_ = {
			h_style::structural::control_width
			+ (h_style::structural::base::pad * 2),
			h_style::structural::base::block_height
		};
	}

	blocks_count label::render(uint64_t tick, LPPOINT cursor_pos)
	{
		gui_manager::renderer->DrawStringC(text,
		                                   h_style::theme::text::font_size_m,
		                                   {origin_.x + h_style::structural::base::margin, origin_.y},
		                                   h_style::theme::colors::base::fg);

		return 1;
	}

	toggle::toggle(bool* data, std::wstring label, const std::function<void()>& on_enabled,
	               const std::function<void()>& on_disabled) : control(), state()
	{
		text = std::move(label);
		state = data;
		size_ = {
			h_style::structural::control_width
			+ (h_style::structural::base::pad * 2),
			h_style::structural::base::block_height
		};

		//default to off position
		marker_offset = {
			size_.x - h_style::structural::control::toggle_marker_width + size_.y / 2, size_.y / 2
		};

		if (on_enabled != nullptr)
		{
			this->on_enable = globals::invoker->add_func([on_enabled](std::any n) { on_enabled(); });
		}

		if (on_enabled != nullptr)
		{
			this->on_disable = globals::invoker->add_func([on_disabled](std::any n) { on_disabled(); });
		}
	}

	blocks_count toggle::render(uint64_t tick, LPPOINT cursor_pos)
	{

		{
			marker_fg = anim::lerp_colf(
				marker_fg, *state
					           ? h_style::theme::colors::control::toggle_marker_on
					           : h_style::theme::colors::control::toggle_marker_off, toggle_anim_alpha);
			marker_bg = anim::lerp_colf(
				marker_bg, *state
					           ? h_style::theme::colors::control::toggle_bg_on
					           : h_style::theme::colors::control::toggle_bg_off, toggle_anim_alpha);

			static D2D1_POINT_2F on_offset = {size_.x - size_.y / 2, size_.y / 2};
			static D2D1_POINT_2F off_offset = {
				size_.x - h_style::structural::control::toggle_marker_width + size_.y / 2, size_.y / 2
			};

			marker_offset = anim::lerp_2f(marker_offset, *state ? on_offset : off_offset, toggle_anim_alpha);

			gui_manager::renderer->DrawCustomRoundedRect(
				D2D1::RoundedRect(
					{
						origin_.x + size_.x - h_style::structural::control::toggle_marker_width,
						origin_.y,
						origin_.x + size_.x,
						origin_.y + size_.y
					},
					size_.y / 2, size_.y / 2
				), true,
				marker_bg,
				0, {}
			);


			gui_manager::renderer->DrawCustomEllipse(
				{
					origin_.x + marker_offset.x,
					origin_.y + marker_offset.y
				}, size_.y / 2 - h_style::structural::base::pad, size_.y / 2 - h_style::structural::base::pad, true,
				marker_fg, 0, {}
			);

			gui_manager::renderer->DrawStringC(text,
				h_style::theme::text::font_size_m,
				{
					origin_.x + h_style::structural::base::margin,
					origin_.y
				}, hovered_
				? h_style::theme::colors::base::fg_hi
				: h_style::theme::colors::base::fg);
		}


		if (hovered_ && enabled_ && gui_manager::input->IsMouseButtonJustPressed(DiInputManager::vM_LEFTBTN))
		{
			*state = !*state;
			globals::invoker->invoke(*state ? this->on_enable : this->on_disable, std::any{});
		}


		return 1;
	}

	button::button(std::wstring label, const std::function<void()>& action) : control()
	{
		text = std::move(label);
		size_ = {
			h_style::structural::control_width
			+ (h_style::structural::base::pad * 2),
			h_style::structural::base::block_height * 2
		};

		if (action == nullptr)
		{
			this->action = globals::invoker->add_func([action](std::any n) { LOG("NO ACTION SET FOR BUTTON"); });
		}
		else
		{
			this->action = globals::invoker->add_func([action](std::any n) { action(); });
		}
	}

	blocks_count button::render(uint64_t tick, LPPOINT cursor_pos)
	{
		static D2D1_COLOR_F button_color = h_style::theme::colors::control::button;

		{
			gui_manager::renderer->DrawCustomRoundedRect(
				D2D1::RoundedRect(
					{
						origin_.x + h_style::structural::base::margin,
						origin_.y + h_style::structural::base::pad + h_style::structural::base::margin,
						origin_.x + size_.x - h_style::structural::base::pad,
						origin_.y + size_.y - h_style::structural::base::pad - h_style::structural::base::margin
					},
					h_style::theme::border_radius / 2,
					h_style::theme::border_radius / 2), true, button_color, 1,
				h_style::theme::colors::control::button_stroke);
		}

		{
			gui_manager::renderer->DrawStringCenteredC(text, h_style::theme::text::font_size_m, {
												   origin_.x + (size_.x / 2),
												   origin_.y + (size_.y / 2),
			                                   }, h_style::theme::colors::base::bg);
		}


		if (hovered_ && enabled_)
		{
			if (gui_manager::input->IsMouseButtonJustReleased(DiInputManager::vM_LEFTBTN))
			{
				globals::invoker->invoke(this->action, std::any{});
			}
			else if (gui_manager::input->IsMouseButtonDown(DiInputManager::vM_LEFTBTN))
			{
				button_color = anim::lerp_colf(button_color, h_style::theme::colors::control::button_pressed, 0.75f);
			}
			else
			{
				button_color = anim::lerp_colf(button_color, h_style::theme::colors::control::button_hovered, 0.05f);
			}
		}
		else
		{
			button_color = anim::lerp_colf(button_color, h_style::theme::colors::control::button, 0.05f);
		}


		return 2;
	}


	slider_double::slider_double(double* data, double min, double max, std::wstring label,
	                             const std::function<void(double)>& on_update) : control(),
		data_(data), min_(min), max_(max)
	{
		text = std::move(label);
		size_ = {
			h_style::structural::control_width
			+ (h_style::structural::base::pad * 2),
			h_style::structural::base::block_height * 2
		};

		if (on_update != nullptr)
		{
			this->on_update_ = globals::invoker->add_func([on_update, min](const std::any& n)
			{
				if (!n.has_value())
				{
					ERR("No value on std::any");
					return;
				}
				try
				{
					auto v = std::any_cast<double>(n);
					on_update(v);
				}
				catch (const std::bad_any_cast& e)
				{
					ERR("Bad std::any_cast! : %s", e.what());
				}
			});
		}
	}

	blocks_count slider_double::render(uint64_t tick, LPPOINT cursor_pos)
	{
		{
			static wchar_t buff[256];
			if (swprintf(buff, 256, text.c_str(), *this->data_) < 0)
			{
				ERR("BAD swprintf on format string (slider_double)");
			}
			const std::wstring ws(buff);
			gui_manager::renderer->DrawStringC(ws,
			                                   h_style::theme::text::font_size_m,
			                                   {origin_.x + h_style::structural::base::margin, origin_.y},
			                                   h_style::theme::colors::base::fg);
		}

		{
			// Slider line
			gui_manager::renderer->DrawLineC(
				{
					origin_.x + h_style::structural::base::margin,
					origin_.y + h_style::structural::base::block_height + (h_style::structural::base::margin *
						2)
				},
				{
					origin_.x + size_.x - h_style::structural::base::pad,
					origin_.y + h_style::structural::base::block_height + (h_style::structural::base::margin *
						2)
				},
				1,
				hovered_
					? h_style::theme::colors::control::slider_hovered
					: h_style::theme::colors::control::slider
			);
		}


		const float min_pos_x = origin_.x + (h_style::structural::base::margin * 2);
		const float max_pos_x = origin_.x + size_.x - h_style::structural::base::pad -
			h_style::structural::base::margin;

		{
			// slider handle
			percent_ = (*this->data_ - min_) / (max_ - min_);
			percent_ = percent_ < 0 ? 0 : percent_ > 1 ? 1.0f : percent_;
			float x_pos = min_pos_x + ((max_pos_x - min_pos_x) * percent_);
			gui_manager::renderer->DrawCustomEllipse(
				{
					x_pos,
					origin_.y + h_style::structural::base::block_height + (h_style::structural::base::margin *
						2)
				},
				h_style::structural::base::margin,
				h_style::structural::base::margin,
				true,
				h_style::theme::colors::control::slider_handle, 0, {0, 0, 0, 0}
			);
		}


		if (hovered_ && enabled_)
		{
			if (gui_manager::input->IsMouseButtonDown(DiInputManager::vM_LEFTBTN))
			{
				percent_ = anim::lerp(percent_, (cursor_pos->x - min_pos_x) / (max_pos_x - min_pos_x), 0.9f);
				percent_ = percent_ < 0 ? 0 : percent_ > 1 ? 1.0f : percent_;

				*this->data_ = min_ + ((max_ - min_) * percent_);

				any_data_.emplace<double>(*this->data_);
				globals::invoker->invoke(this->on_update_, any_data_);
			}
		}


		return 2;
	}


	slider_long::slider_long(long* data, long min, long max, std::wstring label,
	                         const std::function<void(long)>& on_update) : control(),
	                                                                       data_(data), min_(min), max_(max)
	                                                                      
	{
		text = std::move(label);
		size_ = {
			h_style::structural::control_width
			+ (h_style::structural::base::pad * 2),
			h_style::structural::base::block_height * 2
		};


		//TODO: set percent based on val initial
		if (on_update != nullptr)
		{
			this->on_update_ = globals::invoker->add_func([on_update, min](const std::any& n)
			{
				if (!n.has_value())
				{
					ERR("No value on std::any");
					return;
				}
				try
				{
					auto v = std::any_cast<long>(n);
					on_update(v);
				}
				catch (const std::bad_any_cast& e)
				{
					ERR("Bad std::any_cast! : %s", e.what());
				}
			});
		}
	}

	blocks_count slider_long::render(uint64_t tick, LPPOINT cursor_pos)
	{
		{
			static wchar_t buff[256];
			if (swprintf(buff, 256, text.c_str(), *this->data_) < 0)
			{
				ERR("BAD swprintf on format string (slider_long)");
			}
			const std::wstring ws(buff);
			gui_manager::renderer->DrawStringC(ws,
			                                   h_style::theme::text::font_size_m,
			                                   {origin_.x + h_style::structural::base::margin, origin_.y},
			                                   h_style::theme::colors::base::fg);
		}

		{
			// Slider line
			gui_manager::renderer->DrawLineC(
				{
					origin_.x + h_style::structural::base::margin,
					origin_.y + h_style::structural::base::block_height + (h_style::structural::base::margin *
						2)
				},
				{
					origin_.x + size_.x - h_style::structural::base::pad,
					origin_.y + h_style::structural::base::block_height + (h_style::structural::base::margin *
						2)
				},
				1,
				hovered_
					? h_style::theme::colors::control::slider_hovered
					: h_style::theme::colors::control::slider
			);
		}


		const float min_pos_x = origin_.x + (h_style::structural::base::margin * 2);
		const float max_pos_x = origin_.x + size_.x - h_style::structural::base::pad -
			h_style::structural::base::margin;

		{
			// slider handle

			percent_ = static_cast<float>(*this->data_ - min_) / static_cast<float>(max_ - min_);
			percent_ = percent_ < 0 ? 0 : percent_ > 1 ? 1.0f : percent_;

			float x_pos = min_pos_x + ((max_pos_x - min_pos_x) * percent_);
			gui_manager::renderer->DrawCustomEllipse(
				{
					x_pos,
					origin_.y + h_style::structural::base::block_height + (h_style::structural::base::margin *
						2)
				},
				h_style::structural::base::margin,
				h_style::structural::base::margin,
				true, h_style::theme::colors::control::slider_handle, 0, {0, 0, 0, 0}

			);
		}


		if (hovered_ && enabled_)
		{
			if (gui_manager::input->IsMouseButtonDown(DiInputManager::vM_LEFTBTN))
			{
				percent_ = anim::lerp(percent_, (cursor_pos->x - min_pos_x) / (max_pos_x - min_pos_x), 0.9f);
				percent_ = percent_ < 0 ? 0 : percent_ > 1 ? 1.0f : percent_;

				*this->data_ = min_ + static_cast<long>(static_cast<float>(max_ - min_) * percent_);

				any_data_.emplace<long>(*this->data_);
				globals::invoker->invoke(this->on_update_, any_data_);
			}
		}


		return 2;
	}

}

// tab
namespace h_gui
{
	tab::tab(std::wstring text) : interactable({0, 0}), text(std::move(text))
	{
		size_ = {h_style::structural::window::control_groups_width, h_style::structural::window::control_groups_height};
	}

	blocks_count tab::render(uint64_t tick, LPPOINT cursor_pos)
	{
		gui_manager::renderer->DrawCustomRect(
			{
				origin_.x,
				origin_.y,
				origin_.x + size_.x,
				origin_.y + size_.y - h_style::structural::space
			}, true, 0, h_style::theme::colors::tab::tab_page, {});


		gui_manager::renderer->DrawCustomRect(
			{
				origin_.x,
				origin_.y + size_.y - h_style::structural::space,
				origin_.x + h_style::structural::space,
				origin_.y + size_.y
			}, true, 0, h_style::theme::colors::tab::tab_page, {});

		gui_manager::renderer->DrawCustomRoundedRect(
			D2D1::RoundedRect(
				{
					origin_.x,
					origin_.y + size_.y - (h_style::structural::space * 2),
					origin_.x + size_.x,
					origin_.y + size_.y
				}, h_style::theme::border_radius, h_style::theme::border_radius),
			true, h_style::theme::colors::tab::tab_page, 0, {});

		blocks_count blocks = 0;
		int group_spaces = 0;
		float vert_offset = 0;
		bool is_second_column = false;
		for (auto& group : groups_)
		{
			D2D1_POINT_2F group_pos =
			{
				origin_.x + h_style::structural::space
				+ (is_second_column ? (h_style::structural::control_width + (h_style::structural::space * 2)) : 0),

				origin_.y + h_style::structural::window::tab_select_height
				+ vert_offset
				+ (group_spaces * h_style::structural::space)
			};


			group->set_origin(group_pos);

			++group_spaces;

			if (!enabled_)
			{
				group->disable();
			}
			else
			{
				group->enable();
				if (!hovered_)
				{
					group->set_hovered(false);
				}
				else
				{
					group->calc_hovered(cursor_pos);
				}
			}

			blocks += group->render(tick, cursor_pos);

			vert_offset += group->get_size().y;

			if (!is_second_column && blocks >= expected_blocks / 2)
			{
				is_second_column = true;
				group_spaces = 0;
				vert_offset = 0;
			}
		}

		// setup for next frame
		expected_blocks = blocks;
		return 0;
	}

	std::shared_ptr<control_group> tab::add_group(std::wstring label)
	{
		std::shared_ptr<control_group> ptr = std::make_shared<control_group>(label);
		this->groups_.emplace_back(ptr);
		return ptr;
	}
}

// tab_group
namespace h_gui
{
	tab_group::tab_group(std::wstring text) : interactable({0, 0}), text(std::move(text))
	{
		size_ = {
			h_style::structural::window::control_groups_width,
			h_style::structural::window::control_groups_height + h_style::structural::window::tab_select_height
		};
	}

	blocks_count tab_group::render(uint64_t tick, LPPOINT cursor_pos)
	{
		gui_manager::renderer->DrawStringC(text,
		                                   h_style::theme::text::font_size_m,
		                                   {origin_.x + (h_style::structural::base::margin * 4), origin_.y},
		                                   hovered_
			                                   ? h_style::theme::colors::base::fg_hi
			                                   : h_style::theme::colors::base::fg);

		gui_manager::renderer->DrawCustomRect(
			{
				origin_.x,
				origin_.y,
				origin_.x + size_.x,
				origin_.y + size_.y - h_style::structural::space
			}, true, 0, h_style::theme::colors::tab::select_bg, {});


		// Shadow
		{
			// gui_manager::renderer->DrawBitmap(gui_manager::res.BT_GRADIENT,
			//                                   {
			// 	                                  origin_.x,
			// 	                                  origin_.y + h_style::structural::window::tab_select_height -
			// 	                                  gui_manager::res.gradient_sz.y,
			// 	                                  origin_.x + size_.x,
			// 	                                  origin_.y + h_style::structural::window::tab_select_height,
			//                                   }, 0.6f);
		}


		float tab_width = h_style::structural::window::control_groups_width / tabs_.size();
		int i = 0;
		std::shared_ptr<tab> select_next = nullptr;
		for (auto& tab : tabs_)
		{
			// draw tab select
			{
				D2D1_RECT_F tab_rect = {
					origin_.x + (tab_width * i) + h_style::structural::base::margin,
					origin_.y,
					origin_.x + (tab_width * i) + tab_width - h_style::structural::base::margin,
					origin_.y + h_style::structural::window::tab_select_height
				};

				//TODO: Center
				D2D1_POINT_2F text_origin = {
					tab_rect.left + (tab_width / 2) - h_style::structural::base::margin,
					tab_rect.top + (h_style::structural::window::tab_select_height / 2) + h_style::structural::base::pad
				};

				if (tab != this->selected_tab_)
				{
					bool select_hovered = cursor_pos->x < tab_rect.right && cursor_pos->x > tab_rect.left && cursor_pos
						->y > tab_rect.top && cursor_pos->y < tab_rect.bottom;

					gui_manager::renderer->DrawStringCenteredC(tab->text, h_style::theme::text::font_size_m,
					                                           text_origin,
					                                           select_hovered
						                                           ? h_style::theme::colors::tab::text_unselected_hov
						                                           : h_style::theme::colors::tab::text_unselected);

					if (select_hovered && gui_manager::input->IsMouseButtonJustReleased(DiInputManager::vM_LEFTBTN))
					{
						select_next = tab;
					}
				}
				else
				{
					//selected tab

					gui_manager::renderer->DrawCustomRect(
						{
							tab_rect.left + h_style::structural::tab::tab_grab_height - 1,
							tab_rect.bottom - h_style::structural::tab::tab_grab_height,
							tab_rect.right - h_style::structural::tab::tab_grab_height + 1,
							tab_rect.bottom + h_style::structural::base::pad

						}
						, true, 0, h_style::theme::colors::tab::select_face, {});

					// gui_manager::renderer->DrawBitmap(
					// 	gui_manager::res.tab_edge_left,
					// 	{
					// 		tab_rect.left,
					// 		tab_rect.bottom - h_style::structural::tab::tab_grab_height,
					// 		tab_rect.left + h_style::structural::tab::tab_grab_height,
					// 		tab_rect.bottom + h_style::structural::base::pad
					// 	});
					//
					// gui_manager::renderer->DrawBitmap(
					// 	gui_manager::res.tab_edge_right,
					// 	{
					// 		tab_rect.right - h_style::structural::tab::tab_grab_height,
					// 		tab_rect.bottom - h_style::structural::tab::tab_grab_height,
					// 		tab_rect.right,
					// 		tab_rect.bottom + h_style::structural::base::pad
					// 	});

					gui_manager::renderer->DrawStringCenteredC(tab->text, h_style::theme::text::font_size_m, text_origin
					                                           , h_style::theme::colors::tab::text_selected);
				}
			}

			++i;

			// only render selected tab contents
			if (tab != this->selected_tab_)
			{
				tab->disable();
				continue;
			}

			// just under the tab select area
			tab->set_origin({origin_.x, origin_.y + h_style::structural::window::tab_select_height});
			if (!enabled_)
			{
				tab->disable();
			}
			else
			{
				tab->enable();
				if (!hovered_)
				{
					tab->set_hovered(false);
				}
				else
				{
					tab->calc_hovered(cursor_pos);
				}
			}

			tab->render(tick, cursor_pos);
		}

		// set next selected tab if tab was selected
		if (select_next != nullptr)
		{
			this->selected_tab_ = select_next;
		}

		return 0;
	}

	std::shared_ptr<tab> tab_group::add_tab(std::wstring label)
	{
		std::shared_ptr<tab> ptr = std::make_shared<tab>(label);
		this->tabs_.emplace_back(ptr);
		if (this->selected_tab_ == nullptr)
		{
			this->selected_tab_ = ptr;
		}

		return ptr;
	}
}

// section
namespace h_gui
{
	section::section(std::wstring text, std::wstring img_path) : interactable({0, 0})
	{
		this->text = text;
		size_ = {
			h_style::structural::control_width,
			h_style::structural::base::block_height * 2
		};
		this->section_tabs = std::make_shared<tab_group>(text);
		this->icon = globals::gui->create_resource_img(img_path);
	}

	blocks_count section::render(uint64_t tick, LPPOINT cursor_pos)
	{
		if (selected_)
		{
			gui_manager::renderer->DrawCustomRoundedRect(
				D2D1::RoundedRect(
					{
						origin_.x,
						origin_.y,
						origin_.x + size_.x - h_style::structural::space,
						origin_.y + size_.y
					}, h_style::theme::border_radius, h_style::theme::border_radius),
				true, h_style::theme::colors::section::selected_bg,
				2, h_style::theme::colors::section::selected_stroke);
		}


		// gui_manager::renderer->DrawBitmap(this->icon,
		//                                   {
		// 	                                  origin_.x + h_style::structural::base::margin,
		// 	                                  origin_.y + (size_.y / 2) - (icon_sz.y / 2),
		// 	                                  origin_.x + h_style::structural::base::margin + icon_sz.x,
		// 	                                  origin_.y + (size_.y / 2) + (icon_sz.y / 2),
		//                                   },
		//                                   1);


		gui_manager::renderer->DrawStringCenteredC(text,
		                                           h_style::theme::text::font_size_L,
		                                           {

			                                           origin_.x + ((size_.x - icon_sz.x) / 2),
			                                           origin_.y + (size_.y / 2)

		                                           },
		                                           selected_
			                                           ? h_style::theme::colors::section::text_selected
			                                           : hovered_
			                                           ? h_style::theme::colors::section::text_unselected_hov
			                                           : h_style::theme::colors::section::text_unselected);


		return 2;
	}

	bool section::was_just_selected(LPPOINT cursor_pos)
	{
		return is_hovered() && gui_manager::input->IsMouseButtonJustReleased(DiInputManager::vM_LEFTBTN);
	}

	std::shared_ptr<tab_group> section::get_tab_group_ptr()
	{
		return this->section_tabs;
	}

	std::shared_ptr<tab> section::add_tab(std::wstring label)
	{
		return this->section_tabs->add_tab(label);
	}

	void section::set_selected(bool selected)
	{
		selected_ = selected;
	}
}

// sidebar_widget
namespace h_gui
{
	sidebar_widget::sidebar_widget(std::wstring text) : interactable({0, 0}), text(std::move(text))
	{
		size_ = {
			h_style::structural::control_width
			+ (h_style::structural::base::pad * 2),
			h_style::structural::base::block_height
		};
	}

	blocks_count sidebar_widget::render(uint64_t tick, LPPOINT cursor_pos)
	{
		gui_manager::renderer->DrawStringC(text,
		                                   h_style::theme::text::font_size_m,
		                                   {origin_.x + h_style::structural::base::margin, origin_.y},
		                                   h_style::theme::colors::base::fg);

		return 3;
	}
}


//modal selector
namespace h_gui
{

	blocks_count controls::selection_button::render(uint64_t tick, LPPOINT cursor_pos)
	{
		static D2D1_COLOR_F button_color = h_style::theme::colors::control::button;

		{
			gui_manager::renderer->DrawCustomRoundedRect(
				D2D1::RoundedRect(
					{
						origin_.x + h_style::structural::base::margin,
						origin_.y + h_style::structural::base::pad + h_style::structural::base::margin,
						origin_.x + size_.x - h_style::structural::base::pad,
						origin_.y + size_.y - h_style::structural::base::pad - h_style::structural::base::margin
					},
					h_style::theme::border_radius / 2,
					h_style::theme::border_radius / 2), true, button_color, 1,
				h_style::theme::colors::control::button_stroke);
		}

		{
			static wchar_t buff[256];
			if (swprintf(buff, 256, fmt.c_str(), this->text.c_str()) < 0)
			{
				ERR("BAD swprintf on format string (modal_selection_button)");
			}
			const std::wstring ws(buff);

			gui_manager::renderer->DrawStringCenteredC(ws, h_style::theme::text::font_size_m, {
												   origin_.x + (size_.x/2),
												   origin_.y + (size_.y/2),
				}, h_style::theme::colors::base::bg);
		}


		if (hovered_ && enabled_)
		{
			if (gui_manager::input->IsMouseButtonJustReleased(DiInputManager::vM_LEFTBTN))
			{
				this->modal_target_window->set_modal(this->modal_ptr);
			}
			else if (gui_manager::input->IsMouseButtonDown(DiInputManager::vM_LEFTBTN))
			{
				button_color = anim::lerp_colf(button_color, h_style::theme::colors::control::button_pressed, 0.75f);
			}
			else
			{
				button_color = anim::lerp_colf(button_color, h_style::theme::colors::control::button_hovered, 0.05f);
			}
		}
		else
		{
			button_color = anim::lerp_colf(button_color, h_style::theme::colors::control::button, 0.05f);
		}
		return 2;
	}

	void controls::selection_button::set_option_text(std::wstring text)
	{
		this->text = std::move(text);
	}

	modal_selector::modal_selector(size_t* data, const std::wstring& text, std::vector<std::wstring> options, std::shared_ptr<window> window):
	interactable({0, 0}, true), data(data), text(text), options(options)
	{
		this->modal_target_window = window;
		size_ = {
		h_style::structural::control_width,
		h_style::structural::base::block_height * (options.size() + 2)
		};
	}

	void modal_selector::bind_to_button(std::shared_ptr<controls::selection_button> ptr)
	{
		this->button_ptr = ptr;
	}

	blocks_count modal_selector::render(uint64_t tick, LPPOINT cursor_pos)
	{

		gui_manager::renderer->DrawCustomRoundedRect(
			D2D1::RoundedRect({ origin_.x, origin_.y, origin_.x + size_.x, origin_.y + size_.y },
				h_style::theme::border_radius, h_style::theme::border_radius),
			true, h_style::theme::colors::base::bg,1, h_style::theme::colors::window::border);



		blocks_count blocks = 0;
		auto get_option_rect = [this, &blocks]()
			{
				D2D1_RECT_F loc = {
				origin_.x,
				origin_.y + blocks * h_style::structural::base::block_height,
				origin_.x + size_.x,
				origin_.y + ((blocks + 1) * h_style::structural::base::block_height)
				};
				return loc;
			};

		D2D1_RECT_F loc = get_option_rect();
		blocks++;

		gui_manager::renderer->DrawStringCenteredC(this->text,
			h_style::theme::text::font_size_m,
			{ loc.left + ((loc.right - loc.left) / 2), loc.top + ((loc.bottom - loc.top) / 2) },
			h_style::theme::colors::base::fg_hi);

		for (size_t i = 0; i < options.size(); ++i)
		{
			loc = get_option_rect();

			gui_manager::renderer->DrawLineC({ loc.left, loc.top + h_style::structural::base::pad }, { loc.right, loc.top + h_style::structural::base::pad }, 1, h_style::theme::colors::window::separator);

			if (enabled_ && hovered_ && windows_utils::is_point_in_rect(loc, cursor_pos) ){

				//{ loc.left, loc.top + h_style::structural::base::pad }, { loc.right, loc.top + h_style::structural::base::pad }
				gui_manager::renderer->DrawCustomRect({ loc.left, loc.top + h_style::structural::base::pad, loc.right, loc.bottom + h_style::structural::base::pad }, true, 0, { 1,1,1,0.05f }, {});

				gui_manager::renderer->DrawStringCenteredC(options[i].c_str(),
					h_style::theme::text::font_size_m,
					{ loc.left + ((loc.right - loc.left) / 2), loc.top + ((loc.bottom - loc.top) / 2) },
					h_style::theme::colors::base::fg_hi);

				if (gui_manager::input->IsMouseButtonJustReleased(DiInputManager::vM_LEFTBTN)) {
					*data = i;
					this->button_ptr->set_option_text(options.at(i));
					this->modal_target_window->end_modal();
				}

			}else
			{
				gui_manager::renderer->DrawStringCenteredC(options[i].c_str(),
					h_style::theme::text::font_size_m,
					{ loc.left + ((loc.right - loc.left) / 2), loc.top + ((loc.bottom - loc.top) / 2) },
					h_style::theme::colors::base::fg
				);
			}




			blocks++;
		}
		loc = get_option_rect();
		gui_manager::renderer->DrawLineC({ loc.left, loc.top + h_style::structural::base::pad }, { loc.right, loc.top + h_style::structural::base::pad }, 1, h_style::theme::colors::window::separator);



		// if (hovered_ && enabled_)
		// {
		// 	if (gui_manager::input->IsMouseButtonJustReleased(DiInputManager::vM_LEFTBTN))
		// 	{
		// 		// Remove modal
		// 		//TODO:: Fade out
		// 		this->modal_target_window->end_modal();
		//
		// 		//update button
		// 		this->button_ptr->set_option_text(options.at(*data));
		// 	}
		// }

		return blocks;

	}
}

// category
namespace h_gui
{
	category::category(std::wstring text, std::shared_ptr<window> parent) : interactable({0, 0}), text(std::move(text))
	{
		parent_window = parent;
		size_ = {
			h_style::structural::control_width
			+ (h_style::structural::base::pad * 2),
			h_style::structural::base::block_height
		};
	}

	blocks_count category::render(uint64_t tick, LPPOINT cursor_pos)
	{
		blocks_count blocks = 1;
		auto selected_section = parent_window->get_selected_section();

		for (auto& sec : sections_)
		{
			float vert_offset = (blocks * (h_style::structural::base::block_height +
				h_style::structural::base::pad)) - h_style::structural::base::pad;

			sec->set_origin({origin_.x + h_style::structural::base::margin, origin_.y + vert_offset});
			sec->set_selected(selected_section == sec);

			if (!enabled_)
			{
				sec->disable();
			}
			else
			{
				sec->enable();
				if (!hovered_)
				{
					sec->set_hovered(false);
				}
				else
				{
					sec->calc_hovered(cursor_pos);
					if (sec->was_just_selected(cursor_pos))
					{
						parent_window->set_selected_tab_group(sec);
					}
				}
			}

			blocks += sec->render(tick, cursor_pos);
		}
		size_.y = (blocks * h_style::structural::base::block_height);


		return blocks;
	}

	std::shared_ptr<section> category::add_section(std::wstring text, std::wstring img_path)
	{
		std::shared_ptr<section> ptr = std::make_shared<section>(text, img_path);
		this->sections_.emplace_back(ptr);


		// Set the default selected section
		if (parent_window->get_selected_section() == nullptr)
		{
			parent_window->set_selected_tab_group(ptr);
		}

		return ptr;
	}
}

// window
namespace h_gui
{
	blocks_count h_gui::window::render(uint64_t tick, LPPOINT cursor_pos)
	{
		this->enabled_ = true;

		static D2D1_COLOR_F accent_color = h_style::theme::colors::base::accent_a;

		{
			gui_manager::renderer->DrawCustomRoundedRect(
				D2D1::RoundedRect({origin_.x, origin_.y, origin_.x + size_.x, origin_.y + size_.y},
				                  h_style::theme::border_radius,
				                  h_style::theme::border_radius),
				true, h_style::theme::colors::base::bg, 1, h_style::theme::colors::window::border);
		}

		gui_manager::renderer->DrawStringCenteredC(title_,
		                                           h_style::theme::text::font_size_m,
		                                           {
			                                           origin_.x + (size_.x / 2),
			                                           origin_.y + (h_style::structural::window::top_bar_height / 2)
		                                           }, h_style::theme::colors::base::fg);





		if (current_modal_selector != nullptr)
		{
			this->hovered_ = false;
			this->enabled_ = false;
		}

		// render active tab group
		if (this->currently_selected_section != nullptr)
		{
			auto tab_grp = this->currently_selected_section->get_tab_group_ptr();
			tab_grp->set_origin(
				{
					origin_.x + h_style::structural::window::side_bar_width,
					origin_.y + h_style::structural::window::top_bar_height
				});
			tab_grp->render(tick, cursor_pos);

			if(being_dragged || !enabled_)
			{
				tab_grp->disable();
			}
			else {
				tab_grp->enable();
				if (hovered_)
				{
					tab_grp->calc_hovered(cursor_pos);
				}
				else
				{
					tab_grp->set_hovered(false);
				}
			}
		}


		// side bar separator
		{
			gui_manager::renderer->DrawLineC(
				{
					origin_.x + h_style::structural::window::side_bar_width,
					origin_.y + h_style::structural::window::top_bar_height
				},
				{
					origin_.x + h_style::structural::window::side_bar_width,
					origin_.y + size_.y
				},
				2,
				h_style::theme::colors::window::separator);
		}
		// Sidebar shadow
		{
			// gui_manager::renderer->DrawBitmap(gui_manager::res.RL_GRADIENT,
			// 	{
			// 		origin_.x + h_style::structural::window::side_bar_width -
			// 		gui_manager::res.gradient_sz.x,
			// 		origin_.y + h_style::structural::window::top_bar_height,
			// 		origin_.x + h_style::structural::window::side_bar_width,
			// 		origin_.y + size_.y
			// 	}, 0.8f);
		}

		// top bar separator
		{
			float top_line_depth = origin_.y + h_style::structural::window::top_bar_height;
			gui_manager::renderer->DrawLineC(
				{origin_.x, top_line_depth},
				{origin_.x + size_.x, top_line_depth},
				2,
				h_style::theme::colors::window::separator);
		}


		uint16_t blocks_ct = 0; // for the window top bar




		for (const auto& cat : categories_)
		{
			float vert_offset = h_style::structural::window::top_bar_height + (blocks_ct * (
					h_style::structural::base::block_height +
					h_style::structural::base::pad)) +
				h_style::structural::base::pad;
			cat->set_origin({origin_.x + h_style::structural::base::pad, origin_.y + vert_offset});

			if (being_dragged || !this->enabled_)
			{
				cat->disable();
			}
			else
			{
				cat->enable();

				if (!hovered_)
				{
					cat->set_hovered(false);
				}
				else
				{
					cat->calc_hovered(cursor_pos);
				}
			}

			blocks_ct += cat->render(tick, cursor_pos);

			// Separator
			{
				vert_offset = h_style::structural::window::top_bar_height + (blocks_ct * (
						h_style::structural::base::block_height +
						h_style::structural::base::pad)) +
					h_style::structural::base::pad;
				gui_manager::renderer->DrawLineC(
					{
						origin_.x,
						origin_.y + vert_offset + h_style::structural::base::margin
					},
					{
						origin_.x + h_style::structural::window::side_bar_width,
						origin_.y + vert_offset + h_style::structural::base::margin
					},
					2,
					h_style::theme::colors::window::separator);
			}
		}

		if (current_modal_selector != nullptr)
		{
			{
				gui_manager::renderer->DrawCustomRoundedRect(
					D2D1::RoundedRect({ origin_.x, origin_.y, origin_.x + size_.x, origin_.y + size_.y },
						h_style::theme::border_radius,
						h_style::theme::border_radius),
					true, h_style::theme::colors::window::blocked_by_modal, 1, h_style::theme::colors::window::border);
			}
			current_modal_selector->set_origin(
				{
					origin_.x + (size_.x/2) - (h_style::structural::control_width/2),
					origin_.y + (size_.y/2) - (current_modal_selector->get_size().y/2),
				});

			current_modal_selector->calc_hovered(cursor_pos);
			current_modal_selector->render(tick, cursor_pos);
		}



		// Handle Dragging
		{
			if ((hovered_ || being_dragged) && this->enabled_)
			{
				//accent_color = anim::lerp_colf(accent_color, h_style::theme::colors::base::accent_b, 0.025f);
				if (cursor_pos->y < origin_.y + h_style::structural::base::block_height || being_dragged)
				{
					if (gui_manager::input->IsMouseButtonDown(DiInputManager::vM_LEFTBTN))
					{
						if (gui_manager::input->IsMouseButtonJustPressed(DiInputManager::vM_LEFTBTN))
						{
							// store relative point
							being_dragged = true;
							drag_anchor_ = {
								cursor_pos->x - this->origin_.x,
								cursor_pos->y - this->origin_.y
							};
						}

						this->origin_ = anim::lerp_2f(
							this->origin_,
							{cursor_pos->x - drag_anchor_.x, cursor_pos->y - drag_anchor_.y},
							0.55f);
					}
					else
					{
						being_dragged = false;
					}
				}
			}
			else
			{
				//accent_color = anim::lerp_colf(accent_color, h_style::theme::colors::base::accent_a, 0.025f);
			}
		}

		return blocks_ct;
	}

	std::shared_ptr<category> window::add_category(const std::wstring& label)
	{
		std::shared_ptr<category> ptr = std::make_shared<category>(label, shared_from_this());

		this->categories_.emplace_back(ptr);
		return ptr;
	}

	void window::set_selected_tab_group(std::shared_ptr<section> section)
	{
		this->currently_selected_section = section;
	}

	std::shared_ptr<section> window::get_selected_section()
	{
		return this->currently_selected_section;
	}

	void window::set_modal(std::shared_ptr<modal_selector> ptr)
	{
		this->current_modal_selector = ptr;
	}

	void window::end_modal()
	{
		this->current_modal_selector = nullptr;
	}



}

// control_group
namespace h_gui
{
	std::shared_ptr<control> control_group::label(const std::wstring& text)
	{
		std::shared_ptr<control> c = std::make_shared<h_gui::controls::label>(text);
		this->controls_.emplace_back(c);
		return c;
	}

	std::shared_ptr<control> control_group::toggle(bool* data, std::wstring label,
	                                               const std::function<void()>& on_enabled,
	                                               const std::function<void()>& on_disabled)
	{
		std::shared_ptr<control> c = std::make_shared<h_gui::controls::toggle>(data, label, on_enabled, on_disabled);
		this->controls_.emplace_back(c);
		return c;
	}

	std::shared_ptr<control> control_group::slider_double(double* data, double min, double max, std::wstring label,
	                                                      const std::function<void(double)>& on_update)
	{
		std::shared_ptr<control> c = std::make_shared<h_gui::controls::slider_double>(data, min, max, label, on_update);
		this->controls_.emplace_back(c);
		return c;
	}

	std::shared_ptr<control> control_group::slider_long(long* data, long min, long max, std::wstring label,
	                                                    const std::function<void(long)>& on_update)
	{
		std::shared_ptr<control> c = std::make_shared<h_gui::controls::slider_long>(data, min, max, label, on_update);
		this->controls_.emplace_back(c);
		return c;
	}

	std::shared_ptr<control> control_group::button(std::wstring label, const std::function<void()>& action)
	{
		std::shared_ptr<control> c = std::make_shared<h_gui::controls::button>(label, action);
		this->controls_.emplace_back(c);
		return c;
	}

	std::shared_ptr<control> control_group::modal_selection(size_t* data, std::wstring button_label_fmt,
		std::wstring modal_label, std::vector<std::wstring> options, std::shared_ptr<window> modal_target)
	{
		std::shared_ptr<modal_selector> s = std::make_shared<modal_selector>(data, modal_label, options, modal_target);
		auto btn = std::make_shared<controls::selection_button>(options.at(0), button_label_fmt, s, modal_target);
		std::shared_ptr<control> c = btn;
		s->bind_to_button(btn);
		this->controls_.emplace_back(c);
		this->modals.emplace_back(s);
		return c;
	}


	blocks_count control_group::render(uint64_t tick, LPPOINT cursor_pos)
	{
		size_ = {
			h_style::structural::control_width
			+ (h_style::structural::base::margin * 2)
			+ (h_style::structural::base::pad * 2),
			(blocks_ * (h_style::structural::base::block_height + h_style::structural::base::margin))
			+ h_style::structural::base::margin
		};


		gui_manager::renderer->DrawStringC(label_,
		                                   h_style::theme::text::font_size_m,
		                                   {
			                                   origin_.x + (h_style::structural::base::margin * 2) +
			                                   h_style::structural::base::pad,
			                                   origin_.y
		                                   }, h_style::theme::colors::base::fg);
		// Border
		{
			auto border_color = h_style::theme::colors::group::border;
			if (hovered_ && enabled_)
			{
				border_color = h_style::theme::colors::group::border_hovered;
			}


			gui_manager::renderer->DrawCustomRoundedRect(
				D2D1::RoundedRect({
					                  origin_.x,
					                  origin_.y,
					                  origin_.x + size_.x,
					                  origin_.y + size_.y
				                  }, h_style::theme::border_radius, h_style::theme::border_radius),
				true, h_style::theme::colors::group::bg, 1, h_style::theme::colors::group::bg_stroke
			);
		}

		blocks_ = 1; // start at 1 for top text

		for (auto& ctrl : controls_)
		{
			float vert_offset = (blocks_ * (h_style::structural::base::block_height +
				h_style::structural::base::margin)) - h_style::structural::base::pad;

			ctrl->set_origin({origin_.x + h_style::structural::base::margin, origin_.y + vert_offset});


			if (!enabled_)
			{
				ctrl->disable();
			}
			else
			{
				ctrl->enable();
				if (!hovered_)
				{
					ctrl->set_hovered(false);
				}
				else
				{
					ctrl->calc_hovered(cursor_pos);
				}
			}
			blocks_ += ctrl->render(tick, cursor_pos);
		}


		return blocks_;
	}
}

// interactable base
namespace h_gui
{
	void interactable::calc_hovered(LPPOINT cursor_pos)
	{
		if (cursor_pos->x < origin_.x - 1 || cursor_pos->y < origin_.y - 1 || cursor_pos->y > size_.y + origin_.y + 1 ||
			cursor_pos
			->x > size_.x + origin_.x + 1)
		{
			hovered_ = false;
			return;
		}
		this->rel_cursor_->x = cursor_pos->x - static_cast<long>(origin_.x);
		this->rel_cursor_->y = cursor_pos->x - static_cast<long>(origin_.x);
		hovered_ = true;
	}

	void interactable::set_hovered(const bool hovered)
	{
		this->hovered_ = hovered;
		this->rel_cursor_->x = 0;
		this->rel_cursor_->y = 0;
	}

	void interactable::set_origin(const D2D1_POINT_2F origin)
	{
		origin_ = origin;
	}

	void interactable::enable()
	{
		enabled_ = true;
	}

	void interactable::disable()
	{
		enabled_ = false;
	}

	bool interactable::is_hovered()
	{
		return hovered_;
	}
}

// renderable base
namespace h_gui
{
	void renderable::set_renderer(Renderer::D2DxOverlay* renderer)
	{
		renderer_ = renderer;
	}
}

// control base
namespace h_gui
{
	control::control() : interactable({0, 0}), renderable()
	{
	}

	blocks_count control::render(uint64_t tick, LPPOINT cursor_pos)
	{
		return 0;
	}


}

// workspace
namespace h_gui
{
	std::shared_ptr<window> workspace::add_window(const std::wstring& title, D2D1_POINT_2F initial_location,
	                                              const bool start_disabled)
	{
		std::shared_ptr<window> w = std::make_shared<window>(title, initial_location, !start_disabled);
		this->windows_.emplace_back(w);
		return w;
	}

	void workspace::render(UINT32 region_width, UINT32 region_height, uint64_t tick, LPPOINT cursor_pos)
	{
		for (const auto& w : windows_)
		{
			// calculate the window hover here
			// then trickle down for non-hovered in window->render
			w->calc_hovered(cursor_pos);
			w->render(tick, cursor_pos);
		}
	}
}

// gui_manager
namespace h_gui
{
	bool gui_manager::render(UINT32 region_width, UINT32 region_height, uint64_t tick, LPPOINT cursor_pos)
	{
		renderer->SetSolidColor({0,0,0,0});
		for (const auto& ws : workspaces_)
		{
			ws->render(region_width, region_height, tick, cursor_pos);
		}
		return true;
	}

	std::shared_ptr<workspace> gui_manager::add_workspace()
	{
		std::shared_ptr<workspace> ws = std::make_shared<workspace>();
		this->workspaces_.emplace_back(ws);
		return ws;
	}

	void gui_manager::init_shared_res()
	{
		auto path = std::filesystem::temp_directory_path().parent_path().parent_path();
		path /= "hGUI";
		if (!std::filesystem::exists(path))
		{
			std::filesystem::create_directories(path);
			// TODO: Copy images here? or maybe just skip all this and load from byte array.
		}
		res.tab_edge_left = gui_manager::renderer->CreateBitmapImageFromFile(path / "tab_edge_left.png");
		res.tab_edge_right = gui_manager::renderer->CreateBitmapImageFromFile(path / "tab_edge_right.png");
		res.RL_GRADIENT = gui_manager::renderer->CreateBitmapImageFromFile(path / "RL_GRADIENT.png");
		res.BT_GRADIENT = gui_manager::renderer->CreateBitmapImageFromFile(path / "BT_GRADIENT.png");
	}

	Renderer::D2DBitmapID gui_manager::create_resource_img(std::wstring img_name)
	{
		auto path = std::filesystem::temp_directory_path().parent_path().parent_path();
		path /= "hGUI";
		if (!std::filesystem::exists(path))
		{
			std::filesystem::create_directories(path);
			// TODO: Copy images here? or maybe just skip all this and load from byte array.
		}

		return gui_manager::renderer->CreateBitmapImageFromFile(path / img_name);
	}

	gui_manager::gui_manager(Renderer::D2DxOverlay* renderer, const std::shared_ptr<DiInputManager>& input_manager)
	{
		h_gui::gui_manager::renderer = renderer;
		h_gui::gui_manager::input = input_manager;
		init_shared_res();
	}

	gui_manager::~gui_manager()
	{
		workspaces_.clear();
	}
}

// async_invoker
namespace h_gui
{
	void async_invoker::invoke_thread_func()
	{
		std::deque<std::pair<size_t, std::any>> thread_local_invoke_queue_ = {};
		bool queue_empty = true;
		while (true)
		{
			if (thread_exit_signal_) { break; }

			// Minimize blocking time by using flag
			std::this_thread::sleep_for(queue_empty ? queue_max_wait_time_ : thread_sleep_time_);

			{
				// Minimize blocking time by just copying the deque if needed
				std::lock_guard<std::mutex> guard(this->queue_mutex_);

				if (invoke_queue_.empty())
				{
					queue_empty = true;
					continue;
				}
				queue_empty = false;
				thread_local_invoke_queue_ = invoke_queue_;
				invoke_queue_.clear();
			}

			while (!thread_local_invoke_queue_.empty())
			{
				if (!func_map_.contains(thread_local_invoke_queue_.front().first))
				{
					ERR("NON-FATAL bad func ID [%d]", thread_local_invoke_queue_.front().first);
					thread_local_invoke_queue_.pop_front();
					continue;
				}
				func_map_.at(thread_local_invoke_queue_.front().first)(thread_local_invoke_queue_.front().second);
				thread_local_invoke_queue_.pop_front();
			}
		}
	}

	void async_invoker::run()
	{
		invoke_thread_ = std::thread(&async_invoker::invoke_thread_func, this);
		thread_spawned_ = true;
	}

	void async_invoker::stop()
	{
		thread_exit_signal_.store(true);
		invoke_thread_.join();
		thread_spawned_ = false;
	}

	invoker_id async_invoker::add_func(std::function<void(std::any)> func)
	{
		s_id_ += 1;

		if (thread_spawned_)
		{
			ERR(
				"Cannot add functions to async invoker once thread has been spawned. Adding safe placeholder for func id: [%d]",
				s_id_);
			func_map_.try_emplace(s_id_, [](std::any i)
			{
			});
			return s_id_;
		}

		func_map_.try_emplace(s_id_, func);
		return s_id_;
	}

	void async_invoker::invoke(invoker_id id, const std::any& arg)
	{
		if (id < 1) { return; }
		std::lock_guard<std::mutex> guard(this->queue_mutex_);
		invoke_queue_.emplace_back(id, arg);
	}
}
