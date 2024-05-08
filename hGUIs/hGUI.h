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
	class group;
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
		void enable();
		void disable();
	};

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
			invoker_id on_enable;
			invoker_id on_disable;

		public:
			toggle(bool* data, std::wstring label,
			       const std::function<void()>& on_enabled = {},
			       const std::function<void()>& on_disabled = {});

			blocks_count render(uint64_t tick, LPPOINT cursor_pos) override;
		};

		class button final : public control
		{
			std::wstring label;
			invoker_id action;

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

			invoker_id on_update_;

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

			invoker_id on_update_;

		public:
			slider_long(long* data, long min, long max, std::wstring label,
			            const std::function<void(long)>& on_update = {});
			blocks_count render(uint64_t tick, LPPOINT cursor_pos) override;
		};
	}

	class group final : public interactable, public renderable
	{
	private:
		uint16_t blocks_{0};
		std::wstring label_;
		bool collapsible_;
		bool named_;

		std::vector<std::shared_ptr<control>> controls_{};

	public:
		group() : interactable({}, true), renderable(), collapsible_(false), named_(false)
		{
		}

		group(std::wstring label, const bool collapsible = false, const bool named = true, const bool enabled = true)
			: interactable({0, 0}, enabled),
			  renderable(),
			  label_(std::move(label)),
			  collapsible_(collapsible),
			  named_(named)
		{
		}

		virtual ~group() = default;
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


	class window : public interactable, public renderable
	{
	private:
		std::vector<std::shared_ptr<group>> groups_{};
		std::wstring title_;
		D2D1_POINT_2F drag_anchor_ = {};
		bool being_dragged = false;

	public:
		window(std::wstring title, const D2D1_POINT_2F& origin, const bool enabled = true)
			: interactable({origin.x, origin.y}, enabled),
			  title_(std::move(title))
		{
			size_ = {
				h_gui_style::structural::control_width
				+ (h_gui_style::structural::base::margin * 4)
				+ (h_gui_style::structural::base::pad * 2),
				0
			};
		}

		blocks_count render(uint64_t tick, LPPOINT cursor_pos) override;
		std::shared_ptr<group> add_group(const std::wstring& label);
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
		inline static invoker_id s_id_ = 0;

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
