#pragma once
#include <any>
#include <functional>
#include <mutex>
#include <thread>
#include <variant>


#include "D2xOverlay.h"
#include "DiInputManager.h"
#include "style.hpp"

namespace h_gui
{
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
		void enable();
		void disable();
		bool is_hovered();
	};

	inline D2D1_POINT_2F interactable::get_size() const
	{
		return size_;
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
			std::wstring label;
			bool* state;
			invoker_id on_enable = 0;
			invoker_id on_disable = 0;

		public:
			toggle(bool* data, std::wstring label,
			       const std::function<void()>& on_enabled = []{},
			       const std::function<void()>& on_disabled = []{});

			blocks_count render(uint64_t tick, LPPOINT cursor_pos) override;
		};

		class button final : public control
		{
			std::wstring label;
			invoker_id action = 0;

		public:
			button(std::wstring label, const std::function<void()>& action = {});
			blocks_count render(uint64_t tick, LPPOINT cursor_pos) override;
		};


		class slider_double final : public control
		{
			// all sliders use 0.0f-1.0f for frontend
			// interpolate between provided min/max based on %
			float percent_ = 0.5f;

			double* data_;
			double min_;
			double max_;
			std::wstring label;
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

			long* data_;
			long min_;
			long max_;
			std::wstring label;
			std::any any_data_ = {};

			invoker_id on_update_ = 0;

		public:
			slider_long(long* data, long min, long max, std::wstring label,
			            const std::function<void(long)>& on_update = {});
			blocks_count render(uint64_t tick, LPPOINT cursor_pos) override;
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
		                                       const std::function<void(double)>& on_update);
		std::shared_ptr<control> slider_long(long* data, long min, long max, std::wstring label,
		                                     const std::function<void(long)>& on_update);
		std::shared_ptr<control> button(std::wstring label, const std::function<void()>& action = {});
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
		std::wstring text;
		bool selected_ = false;
		std::shared_ptr<tab_group> section_tabs = nullptr;
	public:
		section(std::wstring text);
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
		std::shared_ptr<section> add_section(std::wstring text);
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
		std::vector<std::shared_ptr<category>> categories_{};
		std::shared_ptr<sidebar_widget> sb_widget_top = std::make_shared<sidebar_widget>(L"TOP WIDGET");
		std::shared_ptr<sidebar_widget> sb_widget_bottom = std::make_shared<sidebar_widget>(L"BOTTOM WIDGET");
		std::wstring title_;
		D2D1_POINT_2F drag_anchor_ = {};
		bool being_dragged = false;
	public:
		window(std::wstring title, const D2D1_POINT_2F& origin, const bool enabled = true)
			: interactable({origin.x, origin.y}, enabled),
			  title_(std::move(title))
		{
			size_ = {
				h_style::structural::window::width, h_style::structural::window::height};
		}
		blocks_count render(uint64_t tick, LPPOINT cursor_pos) override;
		std::shared_ptr<category> add_category(const std::wstring& label);
		void set_selected_tab_group(std::shared_ptr<section> section);
		std::shared_ptr<section> get_selected_section();
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

	class gui_manager
	{
	private:
		std::vector<std::shared_ptr<workspace>> workspaces_{};

	public:
		inline static Renderer::D2DxOverlay* renderer = nullptr;
		inline static std::shared_ptr<DiInputManager> input = nullptr;


		bool render(UINT32 region_width, UINT32 region_height, uint64_t tick, LPPOINT cursor_pos);
		std::shared_ptr<workspace> add_workspace();

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
