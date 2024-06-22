#pragma once

#include "D2xOverlay.h"
#include "DiInputManager.h"
#include "style.hpp"

namespace h_gui
{
	class modal_hotkey_picker;
	class modal_obj;
	class modal_color_picker;
	class modal_selector;
	class gui_manager;
	class async_invoker;
	class workspace;
	class window;
	class control_group;
	class control;
	typedef uint16_t blocks_count;
	typedef size_t invoker_id;


	namespace globals
	{
		inline std::unique_ptr<h_gui::gui_manager> gui;
		inline std::unique_ptr<h_gui::async_invoker> invoker;
	}


	class interactable
	{
	protected:
		D2D1_POINT_2F size_;
		D2D1_POINT_2F origin_;
		LPPOINT rel_cursor_;
		bool enabled_;
		bool hovered_;

	public:
		explicit interactable(const D2D1_POINT_2F& origin, const bool enabled = true):
			size_({0, 0}), origin_(origin),
			enabled_(enabled),
			hovered_(false)
		{
			rel_cursor_ = new tagPOINT();
		}

	protected:
		~interactable()
		{
			delete rel_cursor_;
		}

	public:
		void calc_hovered(LPPOINT cursor_pos);
		void set_hovered(bool hovered);
		void set_origin(D2D1_POINT_2F origin);
		D2D1_POINT_2F get_size() const;
		void set_size(D2D1_POINT_2F sz);
		void enable();
		void disable();
		bool is_hovered();
	};

	inline D2D1_POINT_2F interactable::get_size() const
	{
		return size_;
	}
	inline void interactable::set_size(const D2D1_POINT_2F sz)
	{
		size_ = sz;
	}

	class renderable
	{
	protected:
		Renderer::D2DxOverlay* renderer_ = nullptr;
		~renderable() = default;

	public:
		virtual blocks_count render(uint64_t tick, LPPOINT cursor_pos) = 0;
		void set_renderer(Renderer::D2DxOverlay* renderer);
	};

	class control : public interactable, public renderable
	{
	public:
		virtual ~control() = default;

		control(const control& other)
			: interactable(other),
			  renderable(other)
		{
		}

		control(control&& other) noexcept
			: interactable(std::move(other)),
			  renderable(std::move(other))
		{
		}

		control();

		control& operator=(const control& other)
		{
			if (this == &other)
				return *this;
			interactable::operator =(other);
			renderable::operator =(other);
			return *this;
		}

		control& operator=(control&& other) noexcept
		{
			if (this == &other)
				return *this;
			interactable::operator =(std::move(other));
			renderable::operator =(std::move(other));
			return *this;
		}

	public:
		virtual blocks_count render(uint64_t tick, LPPOINT cursor_pos) override;
	};

	namespace controls
	{
		class label final : public control
		{
			std::wstring text;
		public:
			label(std::wstring text);
			blocks_count render(uint64_t tick, LPPOINT cursor_pos) override;
		};

		class toggle final : public control
		{
			std::wstring text;
			bool* state;
			invoker_id on_enable = 0;
			invoker_id on_disable = 0;
			D2D1_COLOR_F marker_bg = {h_style::theme::colors::control::toggle_bg_off};
			D2D1_COLOR_F marker_fg = {h_style::theme::colors::control::toggle_marker_off};
			D2D1_POINT_2F marker_offset = {-1, -1};
			inline static const float toggle_anim_alpha = 0.04f;

		public:
			toggle(bool* data, std::wstring label,
			       const std::function<void()>& on_enabled = []
			       {
			       },
			       const std::function<void()>& on_disabled = []
			       {
			       });

			blocks_count render(uint64_t tick, LPPOINT cursor_pos) override;
		};

		class button final : public control
		{
			std::wstring text;
			invoker_id action = 0;
			D2D1_COLOR_F button_color = h_style::theme::colors::control::button;
		public:
			button(std::wstring label, const std::function<void()>& action = {});
			blocks_count render(uint64_t tick, LPPOINT cursor_pos) override;
		};

		class micro_button final : public control
		{
			invoker_id action = 0;
			D2D1_COLOR_F overlay_color;
			D2D1_COLOR_F target_color;

		public:
			micro_button(D2D1_COLOR_F color,const std::function<void()>& action = {});
			blocks_count render(uint64_t tick, LPPOINT cursor_pos) override;
		};

		class slider_double final : public control
		{
			// all sliders use 0.0f-1.0f for frontend
			// interpolate between provided min/max based on %
			float percent_ = 0.5f;
			std::wstring text;
			double* data_;
			double min_;
			double max_;
			std::any any_data_ = {};

			invoker_id on_update_ = 0;

		public:
			slider_double(double* data, double min, double max, std::wstring label,
			              const std::function<void(double)>& on_update = {});
			blocks_count render(uint64_t tick, LPPOINT cursor_pos) override;
		};

		class slider_long final : public control
		{
			// all sliders use 0.0f-1.0f for frontend
			// interpolate between provided min/max based on %
			float percent_ = 0.5f;
			std::wstring text;
			long* data_;
			long min_;
			long max_;
			std::any any_data_ = {};

			invoker_id on_update_ = 0;

		public:
			slider_long(long* data, long min, long max, std::wstring label,
			            const std::function<void(long)>& on_update = {});
			blocks_count render(uint64_t tick, LPPOINT cursor_pos) override;
		};

		class selection_button final : public control
		{
			std::shared_ptr<modal_selector> modal_ptr;
			std::shared_ptr<window> modal_target_window;
			std::wstring fmt;
			std::wstring text;
			D2D1_COLOR_F button_color = h_style::theme::colors::control::button;
		public:
			selection_button(std::wstring label, std::wstring fmt, std::shared_ptr<modal_selector> modal_ptr, const std::shared_ptr<window>& modal_target_window) : control(),
				modal_ptr(modal_ptr), modal_target_window(modal_target_window), fmt(fmt)
			{
				text = std::move(label);
				size_ = {
					h_style::structural::control_width
					+ (h_style::structural::base::pad * 2),
					h_style::structural::base::block_height * 2
				};
			}
			blocks_count render(uint64_t tick, LPPOINT cursor_pos) override;
			void set_option_text(std::wstring text);
		};

		class color_picker_control final : public control
		{
			std::shared_ptr<modal_color_picker> modal_ptr;
			std::shared_ptr<window> modal_target_window;
			std::wstring text;
			D2D1_COLOR_F* data;
		public:
			color_picker_control(D2D1_COLOR_F* data, std::wstring label, std::shared_ptr<modal_color_picker> modal_ptr, const std::shared_ptr<window>& modal_target_window) : control(),
				data(data), modal_ptr(modal_ptr), modal_target_window(modal_target_window)
			{
				text = std::move(label);
				size_ = {
					h_style::structural::control_width
					+ (h_style::structural::base::pad * 2),
					h_style::structural::base::block_height * 2
				};
			}
			blocks_count render(uint64_t tick, LPPOINT cursor_pos) override;
			void set_color(D2D1_COLOR_F color) const;
		};

		class hotkey_picker_control final : public control
		{
			std::shared_ptr<modal_hotkey_picker> modal_ptr;
			std::shared_ptr<window> modal_target_window;
			std::wstring fmt;
			std::wstring text;
			DiInputManager::DiInput* data;
			D2D1_COLOR_F button_color = h_style::theme::colors::control::button;
		public:
			hotkey_picker_control(DiInputManager::DiInput* data, std::wstring label, std::wstring fmt,
				std::shared_ptr<modal_hotkey_picker> modal_ptr, const std::shared_ptr<window>& modal_target_window) : control(),
				modal_ptr(modal_ptr), modal_target_window(modal_target_window), fmt(fmt), data(data)
			{
				text = std::move(label);
				size_ = {
					h_style::structural::control_width
					+ (h_style::structural::base::pad * 2),
					h_style::structural::base::block_height * 2
				};
			}
			blocks_count render(uint64_t tick, LPPOINT cursor_pos) override;
			void set_key(DiInputManager::DiInput key);
		};

	}

	class control_group final : public interactable, public renderable
	{
	private:
		uint16_t blocks_{0};
		std::wstring label_;
		bool collapsible_;
		bool named_;

		std::vector<std::shared_ptr<control>> controls_{};
		std::vector<std::shared_ptr<modal_obj>> modals{};

	public:
		control_group() : interactable({}, true), renderable(), collapsible_(false), named_(false)
		{
		}

		control_group(std::wstring label, const bool collapsible = false, const bool named = true,
		              const bool enabled = true)
			: interactable({0, 0}, enabled),
			  renderable(),
			  label_(std::move(label)),
			  collapsible_(collapsible),
			  named_(named)
		{
		}

		virtual ~control_group() = default;
		blocks_count render(uint64_t tick, LPPOINT cursor_pos) override;

		std::shared_ptr<control> label(const std::wstring& label);
		std::shared_ptr<control> toggle(bool* data, std::wstring label,
		                                const std::function<void()>& on_enabled = {},
		                                const std::function<void()>& on_disabled = {});

		std::shared_ptr<control> slider_double(double* data, double min, double max, std::wstring label,
		                                       const std::function<void(double)>& on_update = nullptr);
		std::shared_ptr<control> slider_long(long* data, long min, long max, std::wstring label,
		                                     const std::function<void(long)>& on_update = nullptr);
		std::shared_ptr<control> button(std::wstring label, const std::function<void()>& action = {});

		std::shared_ptr<control> modal_selection(size_t* data, std::wstring button_label_fmt, 
			std::wstring modal_label, std::vector<std::wstring> options, std::shared_ptr<window> modal_target);

		std::shared_ptr<control> modal_color(D2D1_COLOR_F* data, std::wstring label,  std::shared_ptr<window> modal_target);
		std::shared_ptr<control> modal_hotkey(DiInputManager::DiInput* data, std::wstring button_label_fmt,
		                                      std::wstring modal_label, std::shared_ptr<window> modal_target);
	};

	class modal_obj : public interactable, public renderable
	{
	protected:
		std::shared_ptr<window> modal_target_window;
	public:
		modal_obj(std::shared_ptr<window> window) : interactable({0,0}, true), modal_target_window(window)
		{
		}
	};

	class modal_selector : public modal_obj
	{
		std::wstring text;
		std::vector<std::wstring> options;
		std::shared_ptr<controls::selection_button> button_ptr;
		size_t* data;
		std::shared_ptr<controls::micro_button> cancel_btn_;

	public:
		modal_selector(size_t* data, const std::wstring& text, std::vector<std::wstring> options, std::shared_ptr<window> window);
		void bind_to_button(std::shared_ptr<controls::selection_button> ptr);
		blocks_count render(uint64_t tick, LPPOINT cursor_pos) override;
		void cancel_action();
	};

	class modal_color_picker : public modal_obj
	{
		std::wstring text;
		D2D1_COLOR_F current{};
		D2D1_COLOR_F cursor_preview = { 1,1,1,1 };
		std::shared_ptr<controls::color_picker_control> control_ptr;
		std::vector<std::shared_ptr<controls::slider_double>> sliders_{};
		std::shared_ptr<controls::button> confirm_btn_;
		double slider_bindings[4] = { 0,0,0, 0 };
		void confirm_action() const;
		D2D1_POINT_2F cursor_preview_loc = { -1,-1 };
		bool has_any_slider_changed = false;
		std::shared_ptr<controls::micro_button> cancel_btn_;

	public:
		modal_color_picker(D2D1_COLOR_F def_color, const std::wstring& text, std::shared_ptr<window> window);
		void bind_to_control(std::shared_ptr<controls::color_picker_control> ptr);
		blocks_count render(uint64_t tick, LPPOINT cursor_pos) override;
		void cancel_action();
	};

	class modal_hotkey_picker : public modal_obj
	{
		std::wstring text;
		std::wstring selected_key_name;
		DiInputManager::DiInput current;
		std::shared_ptr<controls::hotkey_picker_control> control_ptr;
		std::shared_ptr<controls::button> confirm_btn_;
		std::shared_ptr<controls::button> select_btn_;
		std::shared_ptr<controls::micro_button> cancel_btn_;
		bool is_in_select_loop = false;
		void confirm_action() const;
		void cancel_action() const;
	public:
		void set_current(DiInputManager::DiInput key);
		modal_hotkey_picker(const std::wstring& text, std::shared_ptr<window> window);
		void bind_to_control(std::shared_ptr<controls::hotkey_picker_control> ptr);
		blocks_count render(uint64_t tick, LPPOINT cursor_pos) override;
	};

	class tab : public interactable, public renderable
	{
		std::vector<std::shared_ptr<control_group>> groups_;
		blocks_count expected_blocks = 0;

	public:
		std::wstring text;
		tab(std::wstring text);
		blocks_count render(uint64_t tick, LPPOINT cursor_pos) override;
		std::shared_ptr<control_group> add_group(std::wstring label);
	};

	class tab_group : public interactable, public renderable
	{
		std::wstring text;
		std::vector<std::shared_ptr<tab>> tabs_;
		std::shared_ptr<tab> selected_tab_ = nullptr;

	public:
		tab_group(std::wstring text);
		blocks_count render(uint64_t tick, LPPOINT cursor_pos) override;
		std::shared_ptr<tab> add_tab(std::wstring label);
	};

	class section : public interactable, public renderable
	{
		float selected_opacity_ = 0.0f;
		float shadow_opacity_ = 0.0f;
		std::wstring text;
		bool selected_ = false;
		std::shared_ptr<tab_group> section_tabs = nullptr;
		const D2D1_POINT_2F icon_sz = {30, 30};
		Renderer::D2DBitmapID icon;

	public:
		section(std::wstring text, std::wstring img_name);
		blocks_count render(uint64_t tick, LPPOINT cursor_pos) override;
		bool was_just_selected(LPPOINT cursor_pos);
		std::shared_ptr<tab_group> get_tab_group_ptr();
		std::shared_ptr<tab> add_tab(std::wstring label);
		void set_selected(bool selected);
	};

	class category : public interactable, public renderable
	{
		std::vector<std::shared_ptr<section>> sections_{};
		std::wstring text;
		std::shared_ptr<window> parent_window = nullptr;

	public:
		category(std::wstring text, std::shared_ptr<window> parent);
		blocks_count render(uint64_t tick, LPPOINT cursor_pos) override;
		std::shared_ptr<section> add_section(std::wstring text, std::wstring img_path);
	};

	class sidebar_widget : public interactable, public renderable
	{
		std::wstring text;

	public:
		sidebar_widget(std::wstring text);
		blocks_count render(uint64_t tick, LPPOINT cursor_pos) override;
	};

	class window : public interactable, public renderable, public std::enable_shared_from_this<window>
	{
	private:
		std::shared_ptr<section> currently_selected_section = nullptr;
		std::shared_ptr<modal_obj> current_modal_ = nullptr;
		std::vector<std::shared_ptr<category>> categories_{};
		std::shared_ptr<sidebar_widget> sb_widget_top = std::make_shared<sidebar_widget>(L"TOP WIDGET");
		std::shared_ptr<sidebar_widget> sb_widget_bottom = std::make_shared<sidebar_widget>(L"BOTTOM WIDGET");
		std::wstring title_;
		D2D1_POINT_2F drag_anchor_ = {};
		bool being_dragged = false;
		std::shared_ptr<controls::micro_button> close_btn_;
		void close();

	public:

		window(std::wstring title, const D2D1_POINT_2F& origin, const bool enabled = true)
			: interactable({origin.x, origin.y}, enabled),
			  title_(std::move(title))
		{
			size_ = {
				h_style::structural::window::width, h_style::structural::window::height
			};
			this->close_btn_ = std::make_shared<controls::micro_button>(h_style::theme::colors::control::cancel_button_fill, [this] {this->close(); });

		}

		blocks_count render(uint64_t tick, LPPOINT cursor_pos) override;
		std::shared_ptr<category> add_category(const std::wstring& label);
		void set_selected_tab_group(std::shared_ptr<section> section);
		std::shared_ptr<section> get_selected_section();
		void set_modal(std::shared_ptr<modal_obj> ptr);
		void end_modal();
	};

	class workspace
	{
	private:
		std::vector<std::shared_ptr<window>> windows_{};

	public:
		std::shared_ptr<window> add_window(const std::wstring& title, D2D1_POINT_2F initial_location,
		                                   bool start_disabled = false);

		void render(UINT32 region_width, UINT32 region_height, uint64_t tick, LPPOINT cursor_pos);
	};

	struct loaded_resources
	{
		Renderer::D2DBitmapID tab_edge_left = 0;
		Renderer::D2DBitmapID tab_edge_right = 0;

		const D2D1_POINT_2F gradient_sz = {30, 30};
		Renderer::D2DBitmapID RL_GRADIENT = 0;
		Renderer::D2DBitmapID BT_GRADIENT = 0;


		const D2D1_POINT_2F rect_sprite_sz = { 30, 30 };
		Renderer::D2DBitmapID RECT_TL_CORNER;
		Renderer::D2DBitmapID RECT_BL_CORNER;
		Renderer::D2DBitmapID RECT_BR_CORNER;
		Renderer::D2DBitmapID RECT_LEFT_BORDER;
		Renderer::D2DBitmapID RECT_BOTTOM_BORDER;


		const D2D1_POINT_2F rect_color_picker_sz = { 250, 250 };
		Renderer::D2DBitmapID COLOR_PICKER_SQUARE;
		Renderer::D2DBitmapID COLOR_PICKER_PREVIEW_BG;
		Renderer::D2DBitmapID COLOR_PICKER_CONTROL_PREVIEW_BG;

		const D2D1_POINT_2F cancel_highlight_sz = { 25, 25 };
		Renderer::D2DBitmapID CANCEL_HIGHLIGHT;
	};

	class gui_manager
	{
	private:
		std::vector<std::shared_ptr<workspace>> workspaces_{};
		inline static bool show_menu = false;

	public:
		inline static Renderer::D2DxOverlay* renderer = nullptr;
		inline static std::shared_ptr<DiInputManager> input = nullptr;
		inline static loaded_resources res = {};
		inline static bool use_input_blocking = true;
		inline static bool use_blur_behind = true; //broken on win11

		inline static void toggle_menu()
		{
			show_menu = !show_menu;
			if (use_blur_behind) { h_gui::globals::gui->renderer->ToggleAcrylicEffect(h_gui::gui_manager::show_menu); }
			if (use_input_blocking) { h_gui::globals::gui->renderer->SetInputInterception(h_gui::gui_manager::show_menu); }
		}

		inline static void set_show_menu(const bool show)
		{
			show_menu = show;
			if (use_blur_behind) { h_gui::globals::gui->renderer->ToggleAcrylicEffect(h_gui::gui_manager::show_menu); }
			if (use_input_blocking) { h_gui::globals::gui->renderer->SetInputInterception(h_gui::gui_manager::show_menu); }
		}

		inline static void set_effects_only(const bool show)
		{
			if (use_blur_behind) { h_gui::globals::gui->renderer->ToggleAcrylicEffect(show); }
			if (use_input_blocking) { h_gui::globals::gui->renderer->SetInputInterception(show); }
		}

		bool render(UINT32 region_width, UINT32 region_height, uint64_t tick, LPPOINT cursor_pos);
		std::shared_ptr<workspace> add_workspace();
		void init_shared_res();
		Renderer::D2DBitmapID create_resource_img(std::wstring img_name);

		gui_manager(Renderer::D2DxOverlay* renderer, const std::shared_ptr<DiInputManager>& input_manager);
		~gui_manager();
	};

	class async_invoker
	{
	private:
		inline static invoker_id s_id_ = 1;

		std::unordered_map<size_t, std::function<void(std::any)>> func_map_ = {};

		std::mutex queue_mutex_;
		std::deque<std::pair<size_t, std::any>> invoke_queue_ = {};


		void invoke_thread_func();

		std::thread invoke_thread_ = {};
		bool thread_spawned_ = false;
		std::atomic<bool> thread_exit_signal_ = false;

		// Thread sleep time (regardless of queue)
		const std::chrono::milliseconds thread_sleep_time_ = std::chrono::milliseconds(100);
		// Thread sleep nothing in queue
		const std::chrono::milliseconds queue_max_wait_time_ = std::chrono::milliseconds(250);

	public:
		async_invoker() = default;
		void run();
		void stop();

		invoker_id add_func(std::function<void(std::any)> func);
		void invoke(invoker_id id, const std::any& arg);
	};
}
