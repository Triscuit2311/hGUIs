#pragma once


namespace anim
{
	constexpr float reference_fps = 60.0f; // FPS to base calculations on

	// scales alpha value based on actual fps, relative to ref FPS
	inline float scaled_alpha(const float alpha, const float fps)
	{
		return fps <= 0 ? 0 : alpha * (reference_fps / fps);
	}

	template <typename T>
	inline T lerp(const T& start, const T& end, float alpha) {
		alpha = std::clamp(alpha, 0.0f, 1.0f);
		return start + (end - start) * alpha;
	}


	inline D2D1_POINT_2F lerp_2f(const D2D1_POINT_2F& start, const D2D1_POINT_2F& end, float alpha)
	{
		alpha = std::clamp(alpha, 0.0f, 1.0f);

		return { start.x + (end.x - start.x) * alpha, start.y + (end.y - start.y) * alpha };
	}

	inline D2D1_COLOR_F lerp_colf(const D2D1_COLOR_F& start, const D2D1_COLOR_F& end, float alpha)
	{
		alpha = std::clamp(alpha, 0.0f, 1.0f);

		return {
			start.r + (end.r - start.r) * alpha,
			start.g + (end.g - start.g) * alpha,
			start.b + (end.b - start.b) * alpha,
			start.a + (end.a - start.a) * alpha
		};
	}

}