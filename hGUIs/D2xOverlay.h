#pragma once
#include "pch.h"


namespace Renderer {

	typedef void(*DirectOverlayCallback)(UINT32 width, UINT32 height);
	void RunOverlayThread(DirectOverlayCallback callback);
	typedef unsigned long D2DBitmapID;

	/**
	D2DxOverlay class for drawing D2D overlays on top of windows.
	*/
	class D2DxOverlay {

	public:
		enum BrushMode
		{
			SOLID,
			LINEAR_GRADIENT,
			RADIAL_GRADIENT,
		};

		enum LinearGradientDirection
		{
			STRAIGHT_VERTICAL,
			STRAIGHT_HORIZONTAL,
			DIAGONAL_TL_BR,
			DIAGONAL_TR_BL
		};

	private:

		const std::wstring WindowName = L"d2win";
		const std::wstring WindowClassName = L"d2cls";
		const std::wstring FontName = L"Lucida Console";
		
		static D2DxOverlay* Instance; // Singleton instance

		// Windows flags used for creating the overlay window
		DWORD WindowFlags = (WS_EX_LAYERED | WS_EX_TRANSPARENT | WS_EX_TOPMOST | WS_EX_TOOLWINDOW | WS_EX_COMPOSITED);



		D2D1_SIZE_U DesktopSize = {};

		// Handles to the overlay and target windows
		HWND OverlayHwnd = nullptr;
		HWND TargetHwnd = nullptr;

		// Direct2D Primary
		ID2D1Factory* m_D2D1Factory = nullptr;
		ID2D1HwndRenderTarget* m_D2D1RenderTarget = nullptr;


		// Solid Brush
		ID2D1SolidColorBrush* m_D2D1SolidColorBrush = nullptr;
		std::queue<D2D1_COLOR_F> m_SolidColorQueue = {};

		// Linear Gradient
		ID2D1LinearGradientBrush* m_D2D1LinearGradientBrush = nullptr;
		ID2D1GradientStopCollection* m_LinearGradientStopsCol = nullptr;
		D2D1_GRADIENT_STOP m_LinearGradientStops[2]{};
		std::queue<std::pair<D2D1_COLOR_F,D2D1_COLOR_F>> m_LinearColorQueue = {};

		// Radial Gradient
		ID2D1RadialGradientBrush* m_D2D1RadialGradientBrush = nullptr;
		ID2D1GradientStopCollection* m_RadialGradientStopsCol = nullptr;
		D2D1_GRADIENT_STOP m_RadialGradientStops[2]{};
		std::queue<std::pair<D2D1_COLOR_F, D2D1_COLOR_F>> m_RadialColorQueue = {};

		// Brush Modes
		BrushMode FillBrushMode = BrushMode::SOLID;
		BrushMode StrokeBrushMode = BrushMode::SOLID;
		LinearGradientDirection m_LinearGradientFillDirection = LinearGradientDirection::STRAIGHT_VERTICAL;
		LinearGradientDirection m_LinearGradientStrokeDirection = LinearGradientDirection::STRAIGHT_VERTICAL;

		// Brush Pointers
		ID2D1Brush* m_FillBrush = nullptr;
		ID2D1Brush* m_StrokeBrush = nullptr;


		// DirectWrite objects
		IDWriteFactory* m_DWriteFactory = nullptr;
		IDWriteTextFormat* m_WriteTextFormat = nullptr;
		IDWriteTextLayout* m_DefaultWriteTextLayout = nullptr;


		// Bitmap factory
		IWICImagingFactory* m_WicFactory = nullptr;
		std::vector<ID2D1Bitmap*> m_BitmapLibrary;


		// Internal Methods
		void InitializeBrushes();
		void InitializeWindow(HWND tWindow);
		void InitializeD2DFactory();
		void SetRenderTargetSize();
		void InitializeWriteFactoryFonts();

	public:

		void PushSolidColor();
		void PopSolidColor();
		void PushLinearColor();
		void PopLinearColor();
		void PushRadialColor();
		void PopRadialColor();


		static inline std::atomic_bool exit = false;
		static HWND EnumHwnd;
		static DirectOverlayCallback DrawLoopCallback;
		HWND GetTargetHwndRef() const;

		D2DxOverlay();

		/**
		* Returns the singleton instance of the D2DxOverlay class.
		*/
		static D2DxOverlay* GetInstance();

		/**
		* Renders the overlay and returns true if successful, false on break.
		*/
		bool RenderLoop() const;

		/**
		* Sets up the overlay window and Direct2D resources.
		* @param tWindow The target window for the overlay.
		*/
		void Initialize(HWND tWindow);

		D2DBitmapID CreateBitmapImageFromFile(const std::wstring& filePath);
		void ToggleAcrylicEffect(bool enable);
		void SetInputInterception(bool enabled);


		/**
		 * @brief Draws a line from the specified origin to the specified destination
		 *
		 * @param origin The starting point of the line.
		 * @param destination The ending point of the line.
		 * @param thickness The thickness of the line.
		 */
		void DrawLine(const D2D1_POINT_2F origin, const D2D1_POINT_2F destination, const float thickness) const;
		void DrawLineC(D2D1_POINT_2F origin, D2D1_POINT_2F destination, float thickness, D2D1_COLOR_F col);

		/**
		* @brief Draws a solid rect with the specified dimensions, no gradient calculations
		*
		* @param rect The dimensions of the box.
		* @param filled If true, the box is filled with the color; otherwise, an outline of the box is drawn.
		* @param stroke The thickness of the outline stroke, if not filled.
		*/
		void DrawSolidRect(D2D1_RECT_F rect, bool filled, float stroke) const;


		/**
		* @brief Draws a solid rounded rectangle with the specified dimensions, no gradient calculations
		*
		* @param rect The dimensions and corner radii of the rounded rectangle to draw.
		* @param filled Whether the rounded rectangle should be filled or not.
		* @param stroke The thickness of the stroke, if not filled. If 0, no stroke is drawn.
		*/
		void DrawSolidRoundedRect(D2D1_ROUNDED_RECT rect, bool filled, float stroke) const;


		/**
		 * @brief Draws an ellipse with the specified origin, width, height, and stroke thickness.
		 *
		 * @param origin The center point of the ellipse.
		 * @param width The width of the ellipse.
		 * @param height The height of the ellipse.
		 * @param filled If true, the ellipse is filled with the color; otherwise, an outline of the ellipse is drawn.
		 * @param stroke The thickness of the ellipse's outline stroke, if not filled.
		 */
		void DrawCustomEllipse(D2D1_POINT_2F origin, float width, float height, bool filled, float stroke) const;

		/**
		* @brief Draws a solid ellipse, no gradient calculations
		*
		* @param origin The center point of the ellipse.
		* @param width The width of the ellipse.
		* @param height The height of the ellipse.
		* @param filled If true, the ellipse is filled with the color; otherwise, an outline of the ellipse is drawn.
		* @param stroke The thickness of the ellipse's outline stroke, if not filled.
		*/
		void DrawSolidEllipse(D2D1_POINT_2F origin, float width, float height, bool filled, float stroke) const;
		void DrawCustomRect(D2D1_RECT_F rect, bool filled, float stroke, D2D1_COLOR_F fill_col,
		                    D2D1_COLOR_F stroke_col);
		void DrawCustomRoundedRect(D2D1_ROUNDED_RECT rect, bool filled = true, D2D1_COLOR_F fill_col = { 1,1,1,1 }, float stroke = 1,
		                           D2D1_COLOR_F stroke_col = { 1,1,1,1 });
		void DrawCustomEllipse(D2D1_POINT_2F origin, float width, float height, bool filled, D2D1_COLOR_F fill_col,
		                       float stroke, D2D1_COLOR_F stroke_col);

		void DrawBitmap(D2DBitmapID bitmapId, D2D1_RECT_F rect, float opacity = 1.0f) const;
		/**
		 * @brief Draws a string of text with the specified font size, origin
		 *
		 * @param str The string of text to draw.
		 * @param strLength Length of the string
		 * @param fontSize The font size to use for the text.
		 * @param origin The origin point where the text should start being drawn.
		 */
		void DrawString(WCHAR const* str, UINT32 strLength, const float fontSize, const D2D1_POINT_2F origin);
		void DrawString(std::wstring str, float fontSize, D2D1_POINT_2F origin);
		void DrawStringC(std::wstring str, float fontSize, D2D1_POINT_2F origin, D2D1_COLOR_F col);
		void DrawStringCenteredC(WCHAR const* str, UINT32 strLength, float fontSize, D2D1_POINT_2F origin,
		                         D2D1_COLOR_F col);
		void DrawStringCenteredC(std::wstring str, float fontSize, D2D1_POINT_2F origin, D2D1_COLOR_F col);


		/**
		 * @brief Sets the brush mode to use for fill rendering.
		 *
		 * @param mode The brush mode to use for fill rendering.
		 */
		void SetFillBrushMode(BrushMode mode);

		/**
		 * @brief Sets the brush mode to use for stroke rendering.
		 *
		 * @param mode The brush mode to use for stroke rendering.
		 */
		void SetStrokeBrushMode(BrushMode mode);


		/**
		 * @brief Sets the direction of the linear gradient to use for fill.
		 *
		 * @param direction The direction of the linear gradient to use.
		 */
		void SetLinearGradientFillDirection(LinearGradientDirection direction);


		/**
		 * @brief Sets the direction of the linear gradient to use for stroke.
		 *
		 * @param direction The direction of the linear gradient to use.
		 */
		void SetLinearGradientStrokeDirection(LinearGradientDirection direction);


		/**
		* @brief Set the gradient parameters used for creating linear gradients in the specified direction.
		*
		* @param rect The rectangular region to which the gradient is applied.
		* @param linearDirection The direction of the linear gradient.
		*/
		void SetGradientParameters(const D2D1_RECT_F rect, const LinearGradientDirection linearDirection) const;


		/**
		* @brief Load a system font as the default font for the Direct2D application.
		*
		* @param fontFamilyName The name of the font family to be loaded as the default.
		* @note The method first checks whether the font is already installed on the system. If not, it will fall back to a default font.
		*/
		void LoadSystemFontAsDefault(const std::wstring& fontFamilyName);


		/**
		 * Sets the brush mode to draw with a solid color.
		 * @param color The color to use for the brush.
		 */
		void SetSolidColor(D2D1_COLOR_F color) const;

		/**
		 * Sets the opacity of a solid color brush.
		 * @param alpha The opacity value to set.
		 */
		void SetSolidOpacity(float alpha) const;

		/**
		 * Sets the brush mode to draw with a linear gradient between two colors.
		 * @param startColor The start color of the gradient.
		 * @param endColor The end color of the gradient.
		 */
		void SetLinearGradientColors(D2D1_COLOR_F startColor, D2D1_COLOR_F endColor);

		/**
		 * Sets the opacity of a linear gradient brush.
		 * @param startAlpha The start opacity value to set.
		 * @param endAlpha The end opacity value to set.
		 */
		void SetLinearGradientOpacity(float startAlpha, float endAlpha);

		/**
		 * Sets the brush mode to draw with a radial gradient between two colors.
		 * @param innerColor The inner color of the gradient.
		 * @param outerColor The outer color of the gradient.
		 */
		void SetRadialGradientColor(D2D1_COLOR_F innerColor, D2D1_COLOR_F outerColor);

		/**
		 * Sets the opacity of a radial gradient brush.
		 * @param innerAlpha The inner opacity value to set.
		 * @param outerAlpha The outer opacity value to set.
		 */
		void SetRadialGradientOpacity(const float innerAlpha, const float outerAlpha);

	};

}