#include "pch.h"
#include "hGUI.h"
#include <format>

#include "animations.hpp"
#include "animations.hpp"
#include "style.hpp"
#include "logging.h"

// controls
namespace h_gui::controls
{
	label::label(std::wstring text) : control(), text(std::move(text))
	{
		size_ = {
			h_style::structural::control_width
			+ (h_style::structural::base::pad * 2),
			h_style::structural::base::block_height
		};
	}

	blocks_count label::render(uint64_t tick, LPPOINT cursor_pos)
	{
		if (hovered_)
		{
			gui_manager::renderer->PushSolidColor();
			gui_manager::renderer->SetSolidColor({1.0, 0, 0, 1});
		}

		gui_manager::renderer->DrawString(text,
		                                  h_style::theme::text::font_size,
		                                  {origin_.x + h_style::structural::base::margin, origin_.y});

		if (hovered_)
		{
			gui_manager::renderer->PopSolidColor();
		}

		return 1;
	}

	toggle::toggle(bool* data, std::wstring label, const std::function<void()>& on_enabled,
	               const std::function<void()>& on_disabled) : control(), label(std::move(label)), state()
	{
		state = data;
		size_ = {
			h_style::structural::control_width
			+ (h_style::structural::base::pad * 2),
			h_style::structural::base::block_height
		};

		this->on_enable = globals::invoker->add_func([on_enabled](std::any n) { on_enabled(); });
		this->on_disable = globals::invoker->add_func([on_disabled](std::any n) { on_disabled(); });
	}

	blocks_count toggle::render(uint64_t tick, LPPOINT cursor_pos)
	{
		{
			D2D1_COLOR_F fill_col = hovered_
				                        ? h_style::theme::colors::control::toggle_hovered
				                        : h_style::theme::colors::control::toggle;

			gui_manager::renderer->DrawCustomRoundedRect(
				D2D1::RoundedRect(
					{
						origin_.x + h_style::structural::base::margin,
						origin_.y + h_style::structural::base::pad,
						origin_.x + h_style::structural::base::block_height - (h_style::structural::base::pad *
							2) + h_style::structural::base::margin,
						origin_.y + size_.y - h_style::structural::base::pad
					},
					h_style::theme::border_radius / 2,
					h_style::theme::border_radius / 2), true, fill_col, 0);

			if (*state)
			{
				gui_manager::renderer->DrawCustomRoundedRect(
					D2D1::RoundedRect(
						{
							origin_.x + (h_style::structural::base::margin) + h_style::structural::base::pad,
							origin_.y + (h_style::structural::base::pad * 2),
							origin_.x + h_style::structural::base::block_height
							- (h_style::structural::base::pad * 3) + h_style::structural::base::margin,
							origin_.y + size_.y - (h_style::structural::base::pad * 2)
						},
						h_style::theme::border_radius / 3,
						h_style::theme::border_radius / 3), false, {}, 1,
					h_style::theme::colors::control::toggle_marker);
			}
		}

		gui_manager::renderer->DrawString(label,
		                                  h_style::theme::text::font_size,
		                                  {
			                                  origin_.x + (h_style::structural::base::margin * 4) + (
				                                  h_style::structural::base::pad * 2),
			                                  origin_.y
		                                  });


		if (hovered_)
		{
			if (gui_manager::input->IsMouseButtonJustPressed(DiInputManager::vM_LEFTBTN))
			{
				*state = !*state;
				globals::invoker->invoke(*state ? this->on_enable : this->on_disable, std::any{});
			}
		}

		return 1;
	}

	button::button(std::wstring label, const std::function<void()>& action) : control(), label(std::move(label))
	{
		size_ = {
			h_style::structural::control_width
			+ (h_style::structural::base::pad * 2),
			h_style::structural::base::block_height * 2
		};

		this->action = globals::invoker->add_func([action](std::any n) { action(); });
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
			//TODO: Center text
			gui_manager::renderer->DrawStringC(label, h_style::theme::text::font_size, {
				                                   origin_.x + (h_style::structural::base::margin * 2),
				                                   origin_.y + h_style::structural::base::pad +
				                                   h_style::structural::base::margin,
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
		data_(data), min_(min), max_(max), label(std::move(label))
	{
		size_ = {
			h_style::structural::control_width
			+ (h_style::structural::base::pad * 2),
			h_style::structural::base::block_height * 2
		};


		//TODO: set percent based on val initial

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

	blocks_count slider_double::render(uint64_t tick, LPPOINT cursor_pos)
	{
		{
			static wchar_t buff[256];
			if (swprintf(buff, 256, label.c_str(), *this->data_) < 0)
			{
				ERR("BAD swprintf on format string (slider_double)");
			}
			const std::wstring ws(buff);
			gui_manager::renderer->DrawString(ws,
			                                  h_style::theme::text::font_size,
			                                  {origin_.x + h_style::structural::base::margin, origin_.y});
		}

		{
			// Slider line
			gui_manager::renderer->PushSolidColor();
			gui_manager::renderer->SetSolidColor(hovered_
				                                     ? h_style::theme::colors::control::slider_hovered
				                                     : h_style::theme::colors::control::slider);
			gui_manager::renderer->DrawLine(
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
				1
			);
			gui_manager::renderer->PopSolidColor();
		}


		const float min_pos_x = origin_.x + (h_style::structural::base::margin * 2);
		const float max_pos_x = origin_.x + size_.x - h_style::structural::base::pad -
			h_style::structural::base::margin;

		{
			// slider handle
			gui_manager::renderer->PushSolidColor();
			gui_manager::renderer->SetSolidColor(h_style::theme::colors::control::slider_handle);
			float x_pos = min_pos_x + ((max_pos_x - min_pos_x) * percent_);

			gui_manager::renderer->DrawSolidEllipse(
				{
					x_pos,
					origin_.y + h_style::structural::base::block_height + (h_style::structural::base::margin *
						2)
				},
				h_style::structural::base::margin,
				h_style::structural::base::margin,
				true,
				0
			);
			gui_manager::renderer->PopSolidColor();
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
	                                                                       data_(data), min_(min), max_(max),
	                                                                       label(std::move(label))
	{
		size_ = {
			h_style::structural::control_width
			+ (h_style::structural::base::pad * 2),
			h_style::structural::base::block_height * 2
		};


		//TODO: set percent based on val initial

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

	blocks_count slider_long::render(uint64_t tick, LPPOINT cursor_pos)
	{
		{
			static wchar_t buff[256];
			if (swprintf(buff, 256, label.c_str(), *this->data_) < 0)
			{
				ERR("BAD swprintf on format string (slider_long)");
			}
			const std::wstring ws(buff);
			gui_manager::renderer->DrawString(ws,
			                                  h_style::theme::text::font_size,
			                                  {origin_.x + h_style::structural::base::margin, origin_.y});
		}

		{
			// Slider line
			gui_manager::renderer->PushSolidColor();
			gui_manager::renderer->SetSolidColor(hovered_
				                                     ? h_style::theme::colors::control::slider_hovered
				                                     : h_style::theme::colors::control::slider);
			gui_manager::renderer->DrawLine(
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
				1
			);
			gui_manager::renderer->PopSolidColor();
		}


		const float min_pos_x = origin_.x + (h_style::structural::base::margin * 2);
		const float max_pos_x = origin_.x + size_.x - h_style::structural::base::pad -
			h_style::structural::base::margin;

		{
			// slider handle
			gui_manager::renderer->PushSolidColor();
			gui_manager::renderer->SetSolidColor(h_style::theme::colors::control::slider_handle);
			float x_pos = min_pos_x + ((max_pos_x - min_pos_x) * percent_);

			gui_manager::renderer->DrawSolidEllipse(
				{
					x_pos,
					origin_.y + h_style::structural::base::block_height + (h_style::structural::base::margin *
						2)
				},
				h_style::structural::base::margin,
				h_style::structural::base::margin,
				true,
				0
			);
			gui_manager::renderer->PopSolidColor();
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
	tab::tab(std::wstring text) : interactable({ 0, 0 }), text(std::move(text))
	{
		size_ = { h_style::structural::window::control_groups_width, h_style::structural::window::control_groups_height };
	}

	blocks_count tab::render(uint64_t tick, LPPOINT cursor_pos)
	{
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
			h_style::structural::control_width
			+ (h_style::structural::base::pad * 2),
			h_style::structural::base::block_height
		};
	}

	blocks_count tab_group::render(uint64_t tick, LPPOINT cursor_pos)
	{
		gui_manager::renderer->DrawStringC(text,
		                                   h_style::theme::text::font_size,
		                                   {origin_.x + (h_style::structural::base::margin * 4), origin_.y},
		                                   hovered_
			                                   ? h_style::theme::colors::base::fg_hi
			                                   : h_style::theme::colors::base::fg);
		return 1;
	}

	std::shared_ptr<tab> tab_group::add_tab(std::wstring label)
	{
		std::shared_ptr<tab> ptr = std::make_shared<tab>(label);
		this->tabs_.emplace_back(ptr);
		return ptr;
	}
}

// section
namespace h_gui
{
	section::section(std::wstring text) : interactable({0, 0})
	{
		this->text = text;
		size_ = {
			h_style::structural::control_width
			+ (h_style::structural::base::pad * 2),
			h_style::structural::base::block_height
		};
		this->section_tabs = std::make_shared<tab_group>(text);
	}

	blocks_count section::render(uint64_t tick, LPPOINT cursor_pos)
	{
		if (selected_)
		{
			gui_manager::renderer->DrawCustomRect(
				{
					origin_.x + h_style::structural::base::margin,
					origin_.y,
					origin_.x + (h_style::structural::base::margin * 2),
					origin_.y + h_style::structural::base::margin
				}, true, 0, {0, 1, 0, 1}, {});
		}

		gui_manager::renderer->DrawStringC(text,
		                                   h_style::theme::text::font_size,
		                                   {origin_.x + (h_style::structural::base::margin * 4), origin_.y},
		                                   hovered_
			                                   ? h_style::theme::colors::base::fg_hi
			                                   : h_style::theme::colors::base::fg);


		return 1;
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
		gui_manager::renderer->DrawString(text,
		                                  h_style::theme::text::font_size,
		                                  {origin_.x + h_style::structural::base::margin, origin_.y});

		return 3;
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
		gui_manager::renderer->DrawStringC(text,
		                                   h_style::theme::text::font_size,
		                                   {origin_.x + h_style::structural::base::margin, origin_.y},
		                                   hovered_
			                                   ? h_style::theme::colors::base::fg_hi
			                                   : h_style::theme::colors::base::fg);

		blocks_count blocks = 2;

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

		size_.y = blocks * h_style::structural::base::block_height;


		return blocks;
	}

	std::shared_ptr<section> category::add_section(std::wstring text)
	{
		std::shared_ptr<section> ptr = std::make_shared<section>(text);
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
		static D2D1_COLOR_F accent_color = h_style::theme::colors::base::accent_a;

		if (!enabled_) { return 0; }
		{
			gui_manager::renderer->DrawCustomRoundedRect(
				D2D1::RoundedRect({origin_.x, origin_.y, origin_.x + size_.x, origin_.y + size_.y},
				                  h_style::theme::border_radius,
				                  h_style::theme::border_radius),
				true, h_style::theme::colors::base::bg, 1, h_style::theme::colors::window::border);
		}


		gui_manager::renderer->DrawStringC(title_,
		                                   h_style::theme::text::font_size,
		                                   {
			                                   origin_.x + (h_style::structural::base::margin * 2),
			                                   origin_.y + (h_style::structural::base::margin * 2)
		                                   }, h_style::theme::colors::base::fg);

		uint16_t blocks_ct = 2; // for the window top bar
		//TODO: Render top bar seperator

		//TODO: render top widget
		//TODO: Render seperator

		for (const auto& cat : categories_)
		{
			float vert_offset = (blocks_ct * (h_style::structural::base::block_height +
					h_style::structural::base::pad)) +
				h_style::structural::base::pad;
			cat->set_origin({origin_.x + h_style::structural::base::pad, origin_.y + vert_offset});

			if (being_dragged)
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

			//TODO: Render seperator
		}
		//TODO: Render BOTTOM WIDGET


		if (this->currently_selected_section != nullptr)
		{
			auto tab_grp = this->currently_selected_section->get_tab_group_ptr();
			tab_grp->set_origin(
				{
					origin_.x + h_style::structural::control_width + h_style::structural::base::margin,
					origin_.y + h_style::structural::control_width + h_style::structural::base::margin
				});
			tab_grp->render(tick, cursor_pos);
		}


		// Handle Dragging
		if (hovered_ || being_dragged)
		{
			accent_color = anim::lerp_colf(accent_color, h_style::theme::colors::base::accent_b, 0.025f);
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
			accent_color = anim::lerp_colf(accent_color, h_style::theme::colors::base::accent_a, 0.025f);
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


	blocks_count control_group::render(uint64_t tick, LPPOINT cursor_pos)
	{
		size_ = {
			h_style::structural::control_width
			+ (h_style::structural::base::margin * 2)
			+ (h_style::structural::base::pad * 2),
			(blocks_ * (h_style::structural::base::block_height + h_style::structural::base::pad))
			+ h_style::structural::base::margin
		};


		gui_manager::renderer->DrawString(label_,
		                                  h_style::theme::text::font_size,
		                                  {
			                                  origin_.x + (h_style::structural::base::margin * 2) +
			                                  h_style::structural::base::pad,
			                                  origin_.y
		                                  });
		// Border
		{
			auto border_color = h_style::theme::colors::group::border;
			if (hovered_ && enabled_)
			{
				border_color = h_style::theme::colors::group::border_hovered;
			}
			gui_manager::renderer->PushSolidColor();
			gui_manager::renderer->SetSolidColor(border_color);


			gui_manager::renderer->DrawLine(
				{
					origin_.x + h_style::structural::base::margin,
					origin_.y + (h_style::theme::text::font_size - h_style::structural::base::margin)
				},
				{
					origin_.x + (h_style::structural::base::margin * 2),
					origin_.y + (h_style::theme::text::font_size - h_style::structural::base::margin)
				}, 1);

			gui_manager::renderer->DrawLine(
				{
					origin_.x + h_style::structural::base::margin,
					origin_.y + (h_style::theme::text::font_size - h_style::structural::base::margin)
				},
				{
					origin_.x + (h_style::structural::base::margin),
					origin_.y + size_.y - h_style::structural::base::margin
				}, 1);

			gui_manager::renderer->DrawLine(
				{
					origin_.x + (h_style::structural::base::margin),
					origin_.y + size_.y - h_style::structural::base::margin
				},
				{
					origin_.x + size_.x,
					origin_.y + size_.y - h_style::structural::base::margin
				}, 1);

			gui_manager::renderer->DrawLine(
				{
					origin_.x + size_.x,
					origin_.y + size_.y - h_style::structural::base::margin
				},
				{
					origin_.x + size_.x,
					origin_.y + (h_style::theme::text::font_size - h_style::structural::base::margin)
				}, 1);

			gui_manager::renderer->DrawLine(
				{
					origin_.x + size_.x,
					origin_.y + (h_style::theme::text::font_size - h_style::structural::base::margin)
				},
				{
					origin_.x + (h_style::structural::base::margin * 2) + (h_style::theme::text::get_text_width(
						label_.length())),
					origin_.y + (h_style::theme::text::font_size - h_style::structural::base::margin)
				}, 1);


			gui_manager::renderer->PopSolidColor();
		}

		blocks_ = 1; // start at 1 for top label

		for (auto& ctrl : controls_)
		{
			float vert_offset = (blocks_ * (h_style::structural::base::block_height +
				h_style::structural::base::pad)) - h_style::structural::base::pad;

			ctrl->set_origin({origin_.x + h_style::structural::base::margin, origin_.y + vert_offset});


			if (!enabled_)
			{
				ctrl->disable();
			}
			else
			{
				ctrl->enable();
			}


			if (!hovered_)
			{
				ctrl->set_hovered(false);
			}
			else
			{
				ctrl->calc_hovered(cursor_pos);
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

	gui_manager::gui_manager(Renderer::D2DxOverlay* renderer, const std::shared_ptr<DiInputManager>& input_manager)
	{
		h_gui::gui_manager::renderer = renderer;
		h_gui::gui_manager::input = input_manager;
		gui_manager::renderer->SetSolidColor(h_style::theme::colors::base::fg);
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
		std::lock_guard<std::mutex> guard(this->queue_mutex_);
		invoke_queue_.emplace_back(id, arg);
	}
}
