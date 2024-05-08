#include "pch.h"
#include "hGUI.h"
#include <format>
#include "style.hpp"
#include "logging.h"


namespace h_gui::controls
{
	label::label(std::wstring text) : control(), text(std::move(text))
	{
		size_ = {
			h_gui_style::structural::control_width
			+ (h_gui_style::structural::base::pad * 2),
			h_gui_style::structural::base::block_height
		};
	}

	blocks_count label::render(uint64_t tick, LPPOINT cursor_pos)
	{
		if (hovered_)
		{
			gui_manager::renderer->PushSolidColor();
			gui_manager::renderer->SetSolidColor({1.0, 0, 0, 1});
		}

		gui_manager::renderer->DrawString(text.c_str(), static_cast<uint32_t>(text.length()),
		                                  h_gui_style::theme::text::font_size,
		                                  {origin_.x + h_gui_style::structural::base::margin, origin_.y});

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
			h_gui_style::structural::control_width
			+ (h_gui_style::structural::base::pad * 2),
			h_gui_style::structural::base::block_height
		};

		this->on_enable = globals::invoker->add_func([on_enabled](std::any n) { on_enabled(); });
		this->on_disable = globals::invoker->add_func([on_disabled](std::any n) { on_disabled(); });
	}

	blocks_count toggle::render(uint64_t tick, LPPOINT cursor_pos)
	{

		{
			gui_manager::renderer->PushSolidColor();
			gui_manager::renderer->SetSolidColor(hovered_
				                                     ? h_gui_style::theme::colors::control::toggle_hovered
				                                     : h_gui_style::theme::colors::control::toggle);

			gui_manager::renderer->DrawCustomRoundedRect(
				D2D1::RoundedRect(
					{
						origin_.x + h_gui_style::structural::base::margin,
						origin_.y + h_gui_style::structural::base::pad,
						origin_.x + h_gui_style::structural::base::block_height - (h_gui_style::structural::base::pad *
							2) + h_gui_style::structural::base::margin,
						origin_.y + size_.y - h_gui_style::structural::base::pad
					},
					h_gui_style::theme::border_radius / 2,
					h_gui_style::theme::border_radius / 2), true, false);
			gui_manager::renderer->PopSolidColor();

			if (*state)
			{
				gui_manager::renderer->PushSolidColor();
				gui_manager::renderer->SetSolidColor(h_gui_style::theme::colors::control::toggle_marker);

				gui_manager::renderer->DrawCustomRoundedRect(
					D2D1::RoundedRect(
						{
							origin_.x + (h_gui_style::structural::base::margin) + h_gui_style::structural::base::pad,
							origin_.y + (h_gui_style::structural::base::pad * 2),
							origin_.x + h_gui_style::structural::base::block_height
							- (h_gui_style::structural::base::pad * 3) + h_gui_style::structural::base::margin,
							origin_.y + size_.y - (h_gui_style::structural::base::pad * 2)
						},
						h_gui_style::theme::border_radius / 3,
						h_gui_style::theme::border_radius / 3), true, false);
				gui_manager::renderer->PopSolidColor();
			}
		}

		gui_manager::renderer->DrawString(label.c_str(), static_cast<uint32_t>(label.length()),
		                                  h_gui_style::theme::text::font_size,
		                                  {
			                                  origin_.x + (h_gui_style::structural::base::margin * 4) + (
				                                  h_gui_style::structural::base::pad * 2),
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
			h_gui_style::structural::control_width
			+ (h_gui_style::structural::base::pad * 2),
			h_gui_style::structural::base::block_height
		};

		this->action = globals::invoker->add_func([action](std::any n) { action(); });
	}

	blocks_count button::render(uint64_t tick, LPPOINT cursor_pos)
	{
		{
			// Slider line
			gui_manager::renderer->PushSolidColor();
			gui_manager::renderer->SetSolidColor(hovered_
				? h_gui_style::theme::colors::control::slider_hovered
				: h_gui_style::theme::colors::control::slider);
			gui_manager::renderer->DrawLine(
				{
					origin_.x + h_gui_style::structural::base::margin,
					origin_.y +h_gui_style::structural::base::margin
				},
				{
					origin_.x + size_.x - h_gui_style::structural::base::pad,
					origin_.y + h_gui_style::structural::base::margin
				},
				1
			);
			gui_manager::renderer->PopSolidColor();
		}


		if (hovered_ && enabled_)
		{
			if (gui_manager::input->IsMouseButtonDown(DiInputManager::vM_LEFTBTN))
			{
				globals::invoker->invoke(this->action, std::any{});
			}
		}


		return 1;
		//return control::render(tick, cursor_pos);
	}


	slider_double::slider_double(double* data, double min, double max, std::wstring label,
	                             const std::function<void(double)>& on_update) : control(),
	                                                                             data_(data), min_(min), max_(max), label(std::move(label))
	{
		size_ = {
			h_gui_style::structural::control_width
			+ (h_gui_style::structural::base::pad * 2),
			h_gui_style::structural::base::block_height * 2
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
			gui_manager::renderer->DrawString(ws.c_str(), ws.length(),
			                                  h_gui_style::theme::text::font_size,
			                                  {origin_.x + h_gui_style::structural::base::margin, origin_.y});
		}

		{
			// Slider line
			gui_manager::renderer->PushSolidColor();
			gui_manager::renderer->SetSolidColor(hovered_
				                                     ? h_gui_style::theme::colors::control::slider_hovered
				                                     : h_gui_style::theme::colors::control::slider);
			gui_manager::renderer->DrawLine(
				{
					origin_.x + h_gui_style::structural::base::margin,
					origin_.y + h_gui_style::structural::base::block_height + (h_gui_style::structural::base::margin *
						2)
				},
				{
					origin_.x + size_.x - h_gui_style::structural::base::pad,
					origin_.y + h_gui_style::structural::base::block_height + (h_gui_style::structural::base::margin *
						2)
				},
				1
			);
			gui_manager::renderer->PopSolidColor();
		}


		const float min_pos_x = origin_.x + (h_gui_style::structural::base::margin * 2);
		const float max_pos_x = origin_.x + size_.x - h_gui_style::structural::base::pad -
			h_gui_style::structural::base::margin;

		{
			// slider handle
			gui_manager::renderer->PushSolidColor();
			gui_manager::renderer->SetSolidColor(h_gui_style::theme::colors::control::slider_handle);
			float x_pos = min_pos_x + ((max_pos_x - min_pos_x) * percent_);

			gui_manager::renderer->DrawSolidEllipse(
				{
					x_pos,
					origin_.y + h_gui_style::structural::base::block_height + (h_gui_style::structural::base::margin *
						2)
				},
				h_gui_style::structural::base::margin,
				h_gui_style::structural::base::margin,
				true,
				0
			);
			gui_manager::renderer->PopSolidColor();
		}


		if (hovered_ && enabled_)
		{
			if (gui_manager::input->IsMouseButtonDown(DiInputManager::vM_LEFTBTN))
			{
				//TODO: LERP
				percent_ = ((cursor_pos->x - min_pos_x) / (max_pos_x - min_pos_x));
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
			h_gui_style::structural::control_width
			+ (h_gui_style::structural::base::pad * 2),
			h_gui_style::structural::base::block_height * 2
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
			gui_manager::renderer->DrawString(ws.c_str(), ws.length(),
			                                  h_gui_style::theme::text::font_size,
			                                  {origin_.x + h_gui_style::structural::base::margin, origin_.y});
		}

		{
			// Slider line
			gui_manager::renderer->PushSolidColor();
			gui_manager::renderer->SetSolidColor(hovered_
				                                     ? h_gui_style::theme::colors::control::slider_hovered
				                                     : h_gui_style::theme::colors::control::slider);
			gui_manager::renderer->DrawLine(
				{
					origin_.x + h_gui_style::structural::base::margin,
					origin_.y + h_gui_style::structural::base::block_height + (h_gui_style::structural::base::margin *
						2)
				},
				{
					origin_.x + size_.x - h_gui_style::structural::base::pad,
					origin_.y + h_gui_style::structural::base::block_height + (h_gui_style::structural::base::margin *
						2)
				},
				1
			);
			gui_manager::renderer->PopSolidColor();
		}


		const float min_pos_x = origin_.x + (h_gui_style::structural::base::margin * 2);
		const float max_pos_x = origin_.x + size_.x - h_gui_style::structural::base::pad -
			h_gui_style::structural::base::margin;

		{
			// slider handle
			gui_manager::renderer->PushSolidColor();
			gui_manager::renderer->SetSolidColor(h_gui_style::theme::colors::control::slider_handle);
			float x_pos = min_pos_x + ((max_pos_x - min_pos_x) * percent_);

			gui_manager::renderer->DrawSolidEllipse(
				{
					x_pos,
					origin_.y + h_gui_style::structural::base::block_height + (h_gui_style::structural::base::margin *
						2)
				},
				h_gui_style::structural::base::margin,
				h_gui_style::structural::base::margin,
				true,
				0
			);
			gui_manager::renderer->PopSolidColor();
		}


		if (hovered_ && enabled_)
		{
			if (gui_manager::input->IsMouseButtonDown(DiInputManager::vM_LEFTBTN))
			{
				//TODO: LERP
				percent_ = ((cursor_pos->x - min_pos_x) / (max_pos_x - min_pos_x));
				percent_ = percent_ < 0 ? 0 : percent_ > 1 ? 1.0f : percent_;

				*this->data_ = min_ + ((max_ - min_) * percent_);

				any_data_.emplace<long>(*this->data_);
				globals::invoker->invoke(this->on_update_, any_data_);
			}
		}


		return 2;
	}
}

// group
namespace h_gui
{
	std::shared_ptr<control> group::label(const std::wstring& text)
	{
		std::shared_ptr<control> c = std::make_shared<h_gui::controls::label>(text);
		this->controls_.emplace_back(c);
		return c;
	}

	std::shared_ptr<control> group::toggle(bool* data, std::wstring label,
	                                       const std::function<void()>& on_enabled,
	                                       const std::function<void()>& on_disabled)
	{
		std::shared_ptr<control> c = std::make_shared<h_gui::controls::toggle>(data, label, on_enabled, on_disabled);
		this->controls_.emplace_back(c);
		return c;
	}

	std::shared_ptr<control> group::slider_double(double* data, double min, double max, std::wstring label,
	                                              const std::function<void(double)>& on_update)
	{
		std::shared_ptr<control> c = std::make_shared<h_gui::controls::slider_double>(data, min, max, label, on_update);
		this->controls_.emplace_back(c);
		return c;
	}

	std::shared_ptr<control> group::slider_long(long* data, long min, long max, std::wstring label,
	                                            const std::function<void(long)>& on_update)
	{
		std::shared_ptr<control> c = std::make_shared<h_gui::controls::slider_long>(data, min, max, label, on_update);
		this->controls_.emplace_back(c);
		return c;
	}

	std::shared_ptr<control> group::button(std::wstring label, const std::function<void()>& action)
	{
		std::shared_ptr<control> c = std::make_shared<h_gui::controls::button>(label, action);
		this->controls_.emplace_back(c);
		return c;
	}

	blocks_count group::render(uint64_t tick, LPPOINT cursor_pos)
	{
		size_ = {
			h_gui_style::structural::control_width
			+ (h_gui_style::structural::base::margin * 2)
			+ (h_gui_style::structural::base::pad * 2),
			(blocks_ * (h_gui_style::structural::base::block_height + h_gui_style::structural::base::pad))
			+ h_gui_style::structural::base::margin
		};


		gui_manager::renderer->DrawString(label_.c_str(), label_.length(),
		                                  h_gui_style::theme::text::font_size,
		                                  {
			                                  origin_.x + (h_gui_style::structural::base::margin * 2) +
			                                  h_gui_style::structural::base::pad,
			                                  origin_.y
		                                  });
		// Border
		{
			auto border_color = h_gui_style::theme::colors::group::border;
			if (hovered_ && enabled_)
			{
				border_color = h_gui_style::theme::colors::group::border_hovered;
			}
			gui_manager::renderer->PushSolidColor();
			gui_manager::renderer->SetSolidColor(border_color);


			gui_manager::renderer->DrawLine(
				{
					origin_.x + h_gui_style::structural::base::margin,
					origin_.y + (h_gui_style::theme::text::font_size - h_gui_style::structural::base::margin)
				},
				{
					origin_.x + (h_gui_style::structural::base::margin * 2),
					origin_.y + (h_gui_style::theme::text::font_size - h_gui_style::structural::base::margin)
				}, 1);

			gui_manager::renderer->DrawLine(
				{
					origin_.x + h_gui_style::structural::base::margin,
					origin_.y + (h_gui_style::theme::text::font_size - h_gui_style::structural::base::margin)
				},
				{
					origin_.x + (h_gui_style::structural::base::margin),
					origin_.y + size_.y - h_gui_style::structural::base::margin
				}, 1);

			gui_manager::renderer->DrawLine(
				{
					origin_.x + (h_gui_style::structural::base::margin),
					origin_.y + size_.y - h_gui_style::structural::base::margin
				},
				{
					origin_.x + size_.x,
					origin_.y + size_.y - h_gui_style::structural::base::margin
				}, 1);

			gui_manager::renderer->DrawLine(
				{
					origin_.x + size_.x,
					origin_.y + size_.y - h_gui_style::structural::base::margin
				},
				{
					origin_.x + size_.x,
					origin_.y + (h_gui_style::theme::text::font_size - h_gui_style::structural::base::margin)
				}, 1);

			gui_manager::renderer->DrawLine(
				{
					origin_.x + size_.x,
					origin_.y + (h_gui_style::theme::text::font_size - h_gui_style::structural::base::margin)
				},
				{
					origin_.x + (h_gui_style::structural::base::margin * 2) + (h_gui_style::theme::text::get_text_width(
						label_.length())),
					origin_.y + (h_gui_style::theme::text::font_size - h_gui_style::structural::base::margin)
				}, 1);


			gui_manager::renderer->PopSolidColor();
		}

		blocks_ = 1; // start at 1 for top label

		for (auto& ctrl : controls_)
		{
			float vert_offset = (blocks_ * (h_gui_style::structural::base::block_height +
				h_gui_style::structural::base::pad)) - h_gui_style::structural::base::pad;

			ctrl->set_origin({origin_.x + h_gui_style::structural::base::margin, origin_.y + vert_offset});


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
		this->rel_cursor_->x = cursor_pos->x - origin_.x;
		this->rel_cursor_->y = cursor_pos->x - origin_.x;
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

// window
namespace h_gui
{
	blocks_count h_gui::window::render(uint64_t tick, LPPOINT cursor_pos)
	{
		if (!enabled_) { return 0; }


		gui_manager::renderer->SetSolidColor(h_gui_style::theme::colors::base::bg);
		gui_manager::renderer->SetFillBrushMode(Renderer::D2DxOverlay::SOLID);
		gui_manager::renderer->DrawCustomRoundedRect(
			D2D1::RoundedRect({origin_.x, origin_.y, origin_.x + size_.x, origin_.y + size_.y},
			                  h_gui_style::theme::border_radius,
			                  h_gui_style::theme::border_radius), true, false);


		gui_manager::renderer->SetSolidColor(h_gui_style::theme::colors::base::fg);
		gui_manager::renderer->DrawLine(
			{origin_.x, origin_.y + h_gui_style::structural::base::block_height},
			{origin_.x + size_.x, origin_.y + h_gui_style::structural::base::block_height},
			h_gui_style::theme::border_stroke);

		// Border
		{
			auto border_color = h_gui_style::theme::colors::window::border;
			if (hovered_)
			{
				border_color = h_gui_style::theme::colors::window::border_hovered;
			}
			gui_manager::renderer->PushSolidColor();
			gui_manager::renderer->SetSolidColor(border_color);
			gui_manager::renderer->DrawCustomRoundedRect(
				D2D1::RoundedRect({origin_.x, origin_.y, origin_.x + size_.x, origin_.y + size_.y},
				                  h_gui_style::theme::border_radius,
				                  h_gui_style::theme::border_radius), false, true);
			gui_manager::renderer->PopSolidColor();
		}

		gui_manager::renderer->DrawString(title_.c_str(), title_.length(),
		                                  h_gui_style::theme::text::font_size,
		                                  {origin_.x + h_gui_style::structural::base::margin, origin_.y});

		uint16_t blocks_ct = 1;
		for (const auto& grp : groups_)
		{
			float vert_offset = (blocks_ct * (h_gui_style::structural::base::block_height +
					h_gui_style::structural::base::pad)) +
				h_gui_style::structural::base::pad;
			grp->set_origin({origin_.x + h_gui_style::structural::base::pad, origin_.y + vert_offset});

			if(being_dragged)
			{
				grp->disable();
			}else
			{
				grp->enable();
			}

			if (!hovered_)
			{
				grp->set_hovered(false);
			}
			else
			{
				grp->calc_hovered(cursor_pos); //TODO: Relative
			}

			blocks_ct += grp->render(tick, cursor_pos);
		}

		size_ = {
			size_.x, (blocks_ct * (h_gui_style::structural::base::block_height + h_gui_style::structural::base::pad)) +
			h_gui_style::structural::base::pad + (h_gui_style::structural::base::margin * 2)
		};

		if (hovered_ || being_dragged)
		{
			if (cursor_pos->y < origin_.y + h_gui_style::structural::base::block_height || being_dragged)
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
					this->origin_ = {cursor_pos->x - drag_anchor_.x, cursor_pos->y - drag_anchor_.y};
				}
				else
				{
					being_dragged = false;
				}
			}
		}


		return blocks_ct;
	}

	std::shared_ptr<group> window::add_group(const std::wstring& label)
	{
		std::shared_ptr<group> g = std::make_shared<group>(label, false, true, true);
		this->groups_.emplace_back(g);
		return g;
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
		thread_exit_signal_ = true;
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
