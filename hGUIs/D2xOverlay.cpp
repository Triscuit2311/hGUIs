#include "pch.h"
#include "D2xOverlay.h"
#include "logging.h"

#include <windows.h>
#include <dwmapi.h>
#pragma comment(lib, "dwmapi.lib")


// Define ACCENT_STATE and ACCENT_POLICY for SetWindowCompositionAttribute
enum ACCENT_STATE {
	ACCENT_DISABLED = 0,
	ACCENT_ENABLE_GRADIENT = 1,
	ACCENT_ENABLE_TRANSPARENTGRADIENT = 2,
	ACCENT_ENABLE_BLURBEHIND = 3,
	ACCENT_ENABLE_ACRYLICBLURBEHIND = 4,
	ACCENT_ENABLE_HOSTBACKDROP = 5, // Windows 10, version 1809 and later
	ACCENT_INVALID_STATE = 6
};

struct ACCENT_POLICY {
	ACCENT_STATE AccentState;
	DWORD AccentFlags;
	DWORD GradientColor;
	DWORD AnimationId;
};

enum WINDOWCOMPOSITIONATTRIB {
	WCA_UNDEFINED = 0,
	WCA_NCRENDERING_ENABLED = 1,
	WCA_NCRENDERING_POLICY = 2,
	WCA_TRANSITIONS_FORCEDISABLED = 3,
	WCA_ALLOW_NCPAINT = 4,
	WCA_CAPTION_BUTTON_BOUNDS = 5,
	WCA_NONCLIENT_RTL_LAYOUT = 6,
	WCA_FORCE_ICONIC_REPRESENTATION = 7,
	WCA_EXTENDED_FRAME_BOUNDS = 8,
	WCA_HAS_ICONIC_BITMAP = 9,
	WCA_THEME_ATTRIBUTES = 10,
	WCA_NCRENDERING_EXILED = 11,
	WCA_NCADORNMENTINFO = 12,
	WCA_EXCLUDED_FROM_LIVEPREVIEW = 13,
	WCA_VIDEO_OVERLAY_ACTIVE = 14,
	WCA_FORCE_ACTIVEWINDOW_APPEARANCE = 15,
	WCA_DISALLOW_PEEK = 16,
	WCA_CLOAK = 17,
	WCA_CLOAKED = 18,
	WCA_ACCENT_POLICY = 19
};

struct WINDOWCOMPOSITIONATTRIBDATA {
	WINDOWCOMPOSITIONATTRIB Attrib;
	PVOID pvData;
	SIZE_T cbData;
};

// Typedef for SetWindowCompositionAttribute function
typedef BOOL(WINAPI* pSetWindowCompositionAttribute)(HWND, WINDOWCOMPOSITIONATTRIBDATA*);



// Static Setup
namespace Renderer {
	// D2DxOverlay instance initialization to null
	D2DxOverlay* D2DxOverlay::Instance = nullptr;

	// EnumHwnd initialization to null
	HWND D2DxOverlay::EnumHwnd = nullptr;

	// DrawLoopCallback initialization to null
	DirectOverlayCallback D2DxOverlay::DrawLoopCallback = nullptr;

	// Implementation of a window procedure that can be used to subclass
	// the window that will be used to render the overlay
	LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
}

//TODO: Seperate stroke and fill brushes

// Public Drawing
namespace Renderer {

	void D2DxOverlay::DrawBitmap(const D2DBitmapID bitmapId, const D2D1_RECT_F rect) const
	{
		if (bitmapId < 0 || bitmapId >= m_BitmapLibrary.size()) {
			ERR("Bad Bitmap ID: %lu", bitmapId);
			return;
		}
		m_D2D1RenderTarget->DrawBitmap(m_BitmapLibrary.at(bitmapId), rect);
	}

	void D2DxOverlay::DrawString(WCHAR const* str, const UINT32 strLength, const float fontSize, const D2D1_POINT_2F origin)
	{
		// Get the client area of the window.
		RECT tagRect;
		GetClientRect(OverlayHwnd, &tagRect);

		// Calculate the maximum width and height of the text.
		const auto maxWidth = static_cast<float>(tagRect.right - tagRect.left);
		const auto maxHeight = static_cast<float>(tagRect.bottom - tagRect.top);

		// Create a text layout for the given string.
		const HRESULT textLayoutResult = m_DWriteFactory->CreateTextLayout(
			str,
			strLength,
			m_WriteTextFormat,
			maxWidth, maxHeight,
			&m_DefaultWriteTextLayout);

		// If the text layout was successfully created, set its font size, color, and draw it.
		if (SUCCEEDED(textLayoutResult))
		{
			m_DefaultWriteTextLayout->SetFontSize(fontSize, { 0, strLength });


			m_D2D1RenderTarget->DrawTextLayout(origin, m_DefaultWriteTextLayout, m_D2D1SolidColorBrush);

			// Release the text layout.
			m_DefaultWriteTextLayout->Release();
			m_DefaultWriteTextLayout = nullptr;
		}
	}

	void D2DxOverlay::DrawLine(const D2D1_POINT_2F origin, const D2D1_POINT_2F destination, const float thickness = 1) const
	{
		m_D2D1RenderTarget->DrawLine(origin, destination, m_FillBrush, thickness);
	}

	void D2DxOverlay::DrawSolidRect(const D2D1_RECT_F rect, const bool filled = true, const float stroke = 0) const
	{
		// Draw filled
		if (filled) {
			m_D2D1RenderTarget->FillRectangle(rect, m_D2D1SolidColorBrush);
		}

		// In case we don't want an outline
		if (stroke <= 0) return;

		// Draw outline
		m_D2D1RenderTarget->DrawRectangle(rect, m_D2D1SolidColorBrush, stroke);
	}

	void D2DxOverlay::DrawSolidRoundedRect(const D2D1_ROUNDED_RECT rect, const bool filled = true, const float stroke = 0) const
	{
		// Draw filled
		if (filled) {
			m_D2D1RenderTarget->FillRoundedRectangle(rect, m_D2D1SolidColorBrush);
		}

		// In case we don't want an outline
		if (stroke <= 0) return;

		// Draw outline
		m_D2D1RenderTarget->DrawRoundedRectangle(rect, m_D2D1SolidColorBrush, stroke);
	}

	void D2DxOverlay::DrawSolidEllipse(const D2D1_POINT_2F origin, const float width, const float height, const bool filled = true, const float stroke = 0) const
	{
		// Draw filled
		if (filled) {
			m_D2D1RenderTarget->FillEllipse(D2D1::Ellipse(origin, width, height), m_D2D1SolidColorBrush);
			return;
		}

		if (stroke <= 0) return;

		// Draw outline
		m_D2D1RenderTarget->DrawEllipse(D2D1::Ellipse(origin, width, height), m_D2D1SolidColorBrush, stroke);
	}

	void D2DxOverlay::DrawCustomRect(const D2D1_RECT_F rect, const bool filled = true, const float stroke = 0) const
	{
		// Draw filled
		if (filled) {
			SetGradientParameters(rect, m_LinearGradientFillDirection);
			m_D2D1RenderTarget->FillRectangle(rect, m_FillBrush);
		}

		// In case we don't want an outline
		if (stroke <= 0) return;

		// Draw outline
		m_D2D1RenderTarget->DrawRectangle(rect, m_StrokeBrush, stroke);
	}

	void D2DxOverlay::DrawCustomRoundedRect(const D2D1_ROUNDED_RECT rect, const bool filled = true, const float stroke = 0) const
	{
		// Draw filled
		if (filled) {
			SetGradientParameters(rect.rect, m_LinearGradientFillDirection);
			m_D2D1RenderTarget->FillRoundedRectangle(rect, m_FillBrush);
		}

		// In case we don't want an outline
		if (stroke <= 0) return;

		// Draw outline
		m_D2D1RenderTarget->DrawRoundedRectangle(rect, m_StrokeBrush, stroke);
	}

	void D2DxOverlay::DrawCustomEllipse(const D2D1_POINT_2F origin, const float width, const float height, const bool filled = true, const float stroke = 0) const
	{
		// Draw filled
		if (filled) {
			SetGradientParameters(
				{ origin.x,origin.y,origin.x + width,origin.y + height },
				m_LinearGradientFillDirection);
			m_D2D1RenderTarget->FillEllipse(D2D1::Ellipse(origin, width, height), m_FillBrush);
			return;
		}

		if (stroke <= 0) return;

		SetGradientParameters(
			{ origin.x,origin.y,origin.x + width,origin.y + height },
			m_LinearGradientStrokeDirection);

		// Draw outline
		m_D2D1RenderTarget->DrawEllipse(D2D1::Ellipse(origin, width, height), m_StrokeBrush, stroke);
	}
}

// Public Setters
namespace Renderer {
	// Mode Set Methods

	void D2DxOverlay::SetFillBrushMode(const BrushMode mode)
	{
		switch (mode)
		{
		case BrushMode::SOLID:
			m_FillBrush = m_D2D1SolidColorBrush;
			break;
		case BrushMode::LINEAR_GRADIENT:
			m_FillBrush = m_D2D1LinearGradientBrush;
			break;
		case BrushMode::RADIAL_GRADIENT:
			m_FillBrush = m_D2D1RadialGradientBrush;
			break;
		default:
			ERR("Bad brush mode passed to SetFillBrushMode(...)");
			return;
		}

		FillBrushMode = mode;
	}

	void D2DxOverlay::SetStrokeBrushMode(const BrushMode mode)
	{
		switch (mode)
		{
		case BrushMode::SOLID:
			m_StrokeBrush = m_D2D1SolidColorBrush;
			break;
		case BrushMode::LINEAR_GRADIENT:
			m_StrokeBrush = m_D2D1LinearGradientBrush;
			break;
		case BrushMode::RADIAL_GRADIENT:
			m_StrokeBrush = m_D2D1RadialGradientBrush;
			break;
		default:
			ERR("Bad brush mode passed to SetStrokeBrushMode(...)");
			return;
		}
		StrokeBrushMode = mode;
	}

	void D2DxOverlay::SetLinearGradientFillDirection(const LinearGradientDirection direction)
	{
		if (direction <= DIAGONAL_TR_BL && direction >= STRAIGHT_VERTICAL)
			m_LinearGradientFillDirection = direction;
	}

	void D2DxOverlay::SetLinearGradientStrokeDirection(const LinearGradientDirection direction)
	{
		if (direction <= DIAGONAL_TR_BL && direction >= STRAIGHT_VERTICAL)
			m_LinearGradientStrokeDirection = direction;
	}

	void D2DxOverlay::LoadSystemFontAsDefault(const std::wstring& fontFamilyName)
	{
		// Creates a text format object used to define text properties
		HRESULT res = m_DWriteFactory->CreateTextFormat(fontFamilyName.c_str(), nullptr, DWRITE_FONT_WEIGHT_NORMAL,
			DWRITE_FONT_STYLE_NORMAL, DWRITE_FONT_STRETCH_NORMAL, 10.0f, L"en-us", &m_WriteTextFormat);
		if (res != S_OK)
		{
			ERR("Tried to load invalid font in LoadSystemFontAsDefault(...)");
		}

	}

	void D2DxOverlay::SetSolidColor(const D2D1_COLOR_F color) const
	{
		m_D2D1SolidColorBrush->SetColor(color);
	}

	void D2DxOverlay::SetSolidOpacity(const float alpha) const
	{
		m_D2D1SolidColorBrush->SetOpacity(alpha);
	}

	void D2DxOverlay::SetLinearGradientColors(const D2D1_COLOR_F startColor, const D2D1_COLOR_F endColor)
	{
		m_LinearGradientStops[0].color = startColor;
		m_LinearGradientStops[1].color = endColor;
	}

	void D2DxOverlay::SetLinearGradientOpacity(const float startAlpha, const float endAlpha)
	{
		m_LinearGradientStops[0].color.a = startAlpha;
		m_LinearGradientStops[1].color.a = endAlpha;
	}

	void D2DxOverlay::SetRadialGradientColor(const D2D1_COLOR_F innerColor, const D2D1_COLOR_F outerColor)
	{
		m_RadialGradientStops[0].color = innerColor;
		m_RadialGradientStops[1].color = outerColor;
	}

	void D2DxOverlay::SetRadialGradientOpacity(const float innerAlpha, const float outerAlpha)
	{
		m_RadialGradientStops[0].color.a = innerAlpha;
		m_RadialGradientStops[1].color.a = outerAlpha;
	}

}

// Internal Renderer
namespace Renderer {

	void D2DxOverlay::SetGradientParameters(const D2D1_RECT_F rect, const LinearGradientDirection linearDirection) const
	{
		switch (FillBrushMode)
		{
		case LINEAR_GRADIENT:
			switch (linearDirection) {
			case STRAIGHT_VERTICAL:
				m_D2D1LinearGradientBrush->SetStartPoint({ rect.left + ((rect.right - rect.left) / 2), rect.top });
				m_D2D1LinearGradientBrush->SetEndPoint({ rect.left + ((rect.right - rect.left) / 2), rect.bottom });
				break;
			case STRAIGHT_HORIZONTAL:
				m_D2D1LinearGradientBrush->SetStartPoint({ rect.left, rect.top + ((rect.bottom - rect.top) / 2) });
				m_D2D1LinearGradientBrush->SetEndPoint({ rect.right, rect.top + ((rect.bottom - rect.top) / 2) });
				break;
			case DIAGONAL_TL_BR:
				m_D2D1LinearGradientBrush->SetStartPoint({ rect.left, rect.top });
				m_D2D1LinearGradientBrush->SetEndPoint({ rect.right, rect.bottom });
				break;
			case DIAGONAL_TR_BL:
				m_D2D1LinearGradientBrush->SetStartPoint({ rect.right, rect.top });
				m_D2D1LinearGradientBrush->SetEndPoint({ rect.left, rect.bottom });
				break;
			default: break;
			}
			break;
		case RADIAL_GRADIENT:
			m_D2D1RadialGradientBrush->SetCenter(
				{
					rect.right + ((rect.left - rect.right) / 2),
					rect.top + ((rect.bottom - rect.top) / 2),
				});
			m_D2D1RadialGradientBrush->SetRadiusX(((rect.left - rect.right) / 2));
			m_D2D1RadialGradientBrush->SetRadiusY(((rect.bottom - rect.top) / 2));
			break;

			// Fallthrough
		case SOLID:
		default: break;
		}
	}

	void D2DxOverlay::Initialize(const HWND tWindow) {

		InitializeWindow(tWindow);

		InitializeD2DFactory();

		InitializeBrushes();

		m_D2D1RenderTarget->SetAntialiasMode(D2D1_ANTIALIAS_MODE_PER_PRIMITIVE);

		InitializeWriteFactoryFonts();

		SetRenderTargetSize();


		SPE("BITMAP CREATION STARTING");

		CoCreateInstance(
			CLSID_WICImagingFactory, nullptr,
			CLSCTX_INPROC_SERVER, IID_PPV_ARGS(&m_WicFactory));

		//m_WicFactory->Release();
	}

	D2DBitmapID D2DxOverlay::CreateBitmapImageFromFile(const std::wstring& filePath)
	{
		ID2D1Bitmap* tBitmap = nullptr;

		IWICBitmapDecoder* decoder = nullptr;
		HRESULT hr = m_WicFactory->CreateDecoderFromFilename(filePath.c_str(), nullptr, GENERIC_READ, WICDecodeMetadataCacheOnLoad, &decoder);
		if (SUCCEEDED(hr))
		{
			IWICBitmapFrameDecode* frame = nullptr;
			hr = decoder->GetFrame(0, &frame);
			if (SUCCEEDED(hr))
			{
				IWICFormatConverter* converter = nullptr;
				hr = m_WicFactory->CreateFormatConverter(&converter);
				if (SUCCEEDED(hr))
				{
					hr = converter->Initialize(frame, GUID_WICPixelFormat32bppPBGRA, WICBitmapDitherTypeNone, nullptr, 0.f, WICBitmapPaletteTypeMedianCut);
					if (SUCCEEDED(hr))
					{

						hr = m_D2D1RenderTarget->CreateBitmapFromWicBitmap(
							converter, nullptr, &tBitmap);
						if (SUCCEEDED(hr))
						{
							INF("Bitmap Creation Successful: %ws", filePath.c_str());
							INF("Bitmap size: (%d, %d)", tBitmap->GetPixelSize().width, tBitmap->GetPixelSize().height);

							this->m_BitmapLibrary.emplace_back(tBitmap);
							return static_cast<D2DBitmapID>(m_BitmapLibrary.size() - 1);
						}else
						{
							ERR("Bitmap Creation failed for: %ws", filePath.c_str());
						}
					}
					converter->Release();
				}
				frame->Release();
			}
			decoder->Release();
		}

		return -1;
	}


	void D2DxOverlay::ToggleAcrylicEffect(bool enable) {
		static HMODULE hUser = LoadLibrary(L"user32.dll");
		ACCENT_POLICY accentPolicy;

		if (enable) {
			accentPolicy = ACCENT_POLICY{ ACCENT_ENABLE_BLURBEHIND, 2, 0x01000000, 0 };
		}
		else {
			accentPolicy = { ACCENT_DISABLED, 2, 0x00000000, 0 };
		}

		WINDOWCOMPOSITIONATTRIBDATA data;
		data.Attrib = WCA_ACCENT_POLICY;
		data.pvData = &accentPolicy;
		data.cbData = sizeof(accentPolicy);


		if (hUser) {
			auto SetWindowCompositionAttribute = reinterpret_cast<pSetWindowCompositionAttribute>(GetProcAddress(hUser, "SetWindowCompositionAttribute"));
			if (SetWindowCompositionAttribute) {
				SetWindowCompositionAttribute(OverlayHwnd, &data);
			}
		}
	}

	void D2DxOverlay::SetInputInterception(bool enabled) {
		if (enabled) {
			SetWindowLongPtr(OverlayHwnd, GWL_EXSTYLE, GetWindowLongPtr(OverlayHwnd, GWL_EXSTYLE) & ~WS_EX_TRANSPARENT);
		}else
		{
			SetWindowLongPtr(OverlayHwnd, GWL_EXSTYLE, GetWindowLongPtr(OverlayHwnd, GWL_EXSTYLE) | WS_EX_TRANSPARENT);

		}
		SetWindowPos(OverlayHwnd, NULL, 0, 0, 0, 0, SWP_FRAMECHANGED | SWP_NOMOVE | SWP_NOSIZE | SWP_NOZORDER);
	}


	void D2DxOverlay::InitializeWindow(const HWND tWindow)
	{
		// Sets the target window to be the provided window handle
		TargetHwnd = tWindow;

		// Defines a window class and sets its properties
		WNDCLASS wc = { };
		wc.lpfnWndProc = WindowProc;
		wc.hInstance = GetModuleHandle(nullptr);
		wc.lpszClassName = WindowClassName.c_str();

		// Registers the window class with the operating system
		RegisterClass(&wc);

		// Creates a window with the specified extended style, class name, and title
		// and sets its position, size, and parent window
		OverlayHwnd = CreateWindowEx(
			WindowFlags,
			wc.lpszClassName, WindowName.c_str(),
			WS_POPUP,
			CW_USEDEFAULT, CW_USEDEFAULT,
			CW_USEDEFAULT, CW_USEDEFAULT,
			nullptr, nullptr,
			wc.hInstance, nullptr);

		// Extends the frame into the client area of the window
		constexpr MARGINS margins = { -1,-1,-1,-1 };
		DwmExtendFrameIntoClientArea(OverlayHwnd, &margins);
	}

	void D2DxOverlay::InitializeD2DFactory()
	{
		// Creates a factory object used to create Direct2D resources
		D2D1CreateFactory(D2D1_FACTORY_TYPE_MULTI_THREADED, &m_D2D1Factory);

		// Creates a render target that represents a window
		m_D2D1Factory->CreateHwndRenderTarget(
			// Specifies the render target properties, such as render target type and pixel format
			D2D1::RenderTargetProperties(D2D1_RENDER_TARGET_TYPE_DEFAULT,
				D2D1::PixelFormat(DXGI_FORMAT_UNKNOWN, D2D1_ALPHA_MODE_PREMULTIPLIED)),
			// Specifies the HWND and size of the window that will be used as the render target
			D2D1::HwndRenderTargetProperties(OverlayHwnd, D2D1::SizeU(200, 200),
				D2D1_PRESENT_OPTIONS_IMMEDIATELY), &m_D2D1RenderTarget);
	}

	void D2DxOverlay::SetRenderTargetSize()
	{
		// Gets the size of the desktop window
		const HDC hDc = ::GetWindowDC(nullptr);
		DesktopSize.width = ::GetDeviceCaps(hDc, HORZRES);
		DesktopSize.height = ::GetDeviceCaps(hDc, VERTRES);

		// Resizes the window to fill the entire desktop
		::SetWindowPos(OverlayHwnd, nullptr, 0, 0, DesktopSize.width, DesktopSize.height, SWP_FRAMECHANGED);

		// Resizes the render target to match the size of the window
		m_D2D1RenderTarget->Resize({ DesktopSize.width, DesktopSize.height });
	}

	void D2DxOverlay::InitializeBrushes()
	{
		// Creates a new solid color brush
		m_D2D1RenderTarget->CreateSolidColorBrush(D2D1::ColorF(0.7f, 0.7f, 0.7f), &m_D2D1SolidColorBrush);


		// Create linear brush
		{
			// Set the colors and positions for the gradient stops
			m_LinearGradientStops[0].color = D2D1::ColorF(D2D1::ColorF::Aquamarine, 1);
			m_LinearGradientStops[0].position = 0.0f;
			m_LinearGradientStops[1].color = D2D1::ColorF(D2D1::ColorF::Crimson, 1);
			m_LinearGradientStops[1].position = 1.0f;

			// Create a gradient stop collection for the linear gradient brush
			m_D2D1RenderTarget->CreateGradientStopCollection(
				m_LinearGradientStops,
				2,
				D2D1_GAMMA_2_2,
				D2D1_EXTEND_MODE_CLAMP,
				&m_LinearGradientStopsCol
			);

			// Create a linear gradient brush using the gradient stop collection and properties
			m_D2D1RenderTarget->CreateLinearGradientBrush(
				D2D1::LinearGradientBrushProperties(
					D2D1::Point2F(0, 0),
					D2D1::Point2F(1000, 1000)),
				m_LinearGradientStopsCol,
				&m_D2D1LinearGradientBrush
			);
		}

		// Create radial brush
		{
			// Set the colors and positions for the gradient stops
			m_RadialGradientStops[0].color = D2D1::ColorF(D2D1::ColorF::BlueViolet, 1);
			m_RadialGradientStops[0].position = 0.0f;
			m_RadialGradientStops[1].color = D2D1::ColorF(D2D1::ColorF::Chartreuse, 1);
			m_RadialGradientStops[1].position = 1.0f;

			// Create a gradient stop collection for the radial gradient brush
			m_D2D1RenderTarget->CreateGradientStopCollection(
				m_RadialGradientStops,
				2,
				D2D1_GAMMA_2_2,
				D2D1_EXTEND_MODE_CLAMP,
				&m_RadialGradientStopsCol
			);

			// Create a radial gradient brush using the gradient stop collection and properties
			m_D2D1RenderTarget->CreateRadialGradientBrush(
				D2D1::RadialGradientBrushProperties({ 0,0 }, { 0,0 }, 100, 100),
				m_RadialGradientStopsCol,
				&m_D2D1RadialGradientBrush);
		}


		// Setup default brushes
		m_FillBrush = m_D2D1SolidColorBrush;
		m_StrokeBrush = m_D2D1SolidColorBrush;
	}

	void D2DxOverlay::InitializeWriteFactoryFonts()
	{
		// Creates a factory object used to create DirectWrite resources
		DWriteCreateFactory(DWRITE_FACTORY_TYPE_SHARED, __uuidof(IDWriteFactory), reinterpret_cast<IUnknown**>(&m_DWriteFactory));

		// Creates a text format object used to define text properties
		m_DWriteFactory->CreateTextFormat(FontName.c_str(), nullptr, DWRITE_FONT_WEIGHT_NORMAL,
			DWRITE_FONT_STYLE_NORMAL, DWRITE_FONT_STRETCH_NORMAL, 10.0f, L"en-us", &m_WriteTextFormat);
	}

	void D2DxOverlay::PushSolidColor()
	{
		m_SolidColorQueue.push(this->m_D2D1SolidColorBrush->GetColor());
	}

	void D2DxOverlay::PopSolidColor()
	{
		if (m_SolidColorQueue.empty())
		{
			ERR("Tried to pop from empty solid color queue");
			return;
		}

		this->m_D2D1SolidColorBrush->SetColor(m_SolidColorQueue.front());
		m_SolidColorQueue.pop();
	}

	void D2DxOverlay::PushLinearColor()
	{
		m_LinearColorQueue.push({ this->m_LinearGradientStops[0].color, this->m_LinearGradientStops[1].color });
	}

	void D2DxOverlay::PopLinearColor()
	{
		if (m_LinearColorQueue.empty())
		{
			ERR("Tried to pop from empty linear color queue");
			return;
		}
		this->m_LinearGradientStops[0].color = m_LinearColorQueue.front().first;
		this->m_LinearGradientStops[1].color = m_LinearColorQueue.front().second;
		m_LinearColorQueue.pop();
	}

	void D2DxOverlay::PushRadialColor()
	{
		m_RadialColorQueue.push({ this->m_RadialGradientStops[0].color, this->m_RadialGradientStops[1].color });
	}

	void D2DxOverlay::PopRadialColor()
	{
		if(m_RadialColorQueue.empty())
		{
			ERR("Tried to pop from empty radial color queue");
			return;
		}
		this->m_RadialGradientStops[0].color = m_RadialColorQueue.front().first;
		this->m_RadialGradientStops[1].color = m_RadialColorQueue.front().second;
		m_RadialColorQueue.pop();
	}

	HWND D2DxOverlay::GetTargetHwndRef() const
	{
		return this->TargetHwnd;
	}

	D2DxOverlay::D2DxOverlay()
	{
		// Check and set singleton instance
		if (Instance == nullptr) {

			Instance = this;
			return;
		}
		return;
	}

	D2DxOverlay* D2DxOverlay::GetInstance()
	{
		return D2DxOverlay::Instance;
	}


	bool D2DxOverlay::RenderLoop() const
	{
		MSG message;
		message.message = WM_NULL;

		ShowWindow(OverlayHwnd, 1);
		SetLayeredWindowAttributes(OverlayHwnd, RGB(0, 0, 0), 255, LWA_ALPHA);


		if (message.message != WM_QUIT)
		{
			// If there are any messages in the message queue, retrieve and process them
			if (PeekMessage(&message, OverlayHwnd, NULL, NULL, PM_REMOVE))
			{
				TranslateMessage(&message);
				DispatchMessage(&message);
			}
			UpdateWindow(OverlayHwnd);

			// Start render
			m_D2D1RenderTarget->BeginDraw();
			m_D2D1RenderTarget->Clear(D2D1::ColorF(0, 0, 0, 0));


			if (DrawLoopCallback != nullptr) {
				DrawLoopCallback(DesktopSize.width, DesktopSize.height);
			}


			m_D2D1RenderTarget->EndDraw();
			Sleep(1); //TODO: Use chrono
		}
		else
		{
			// If WM_QUIT was received, stop rendering and return false
			return false;
		}

		return true;
	}

}



// Window Thread
namespace Renderer {

	LRESULT CALLBACK WindowProc(const HWND hwnd, const UINT uMsg, const WPARAM wParam, const LPARAM lParam)
	{
		// Switch statement to handle different window messages
		switch (uMsg)
		{
		case WM_SETCURSOR:
			// Set the cursor to the default arrow cursor
			SetCursor(LoadCursor(NULL, IDC_ARROW));
			break;
		case WM_CLOSE:
			DestroyWindow(hwnd);
			break;
		case WM_DESTROY:
			// Post a quit message to the message queue, which will cause the GetMessage() function to return FALSE and end the message loop
			PostQuitMessage(0);
			break;
		default:
			// Handle any remaining messages using the default window procedure
			return DefWindowProc(hwnd, uMsg, wParam, lParam);
		}

		// Return 0 to indicate that the message has been processed
		return 0;
	}

	// Callback function to be called for each top-level window enumerated
	// .. during a call to EnumWindows.
	// It is used to find the top-level window that belongs to the current process.
	BOOL CALLBACK EnumWindowsProc(const HWND hwnd, LPARAM lParam)
	{
		// Get the process ID of the specified window
		DWORD pid;
		GetWindowThreadProcessId(hwnd, &pid);

		// Check if the process ID matches the current process ID
		if (pid == GetCurrentProcessId())
		{
			// If the process ID matches, save the window handle
			// and stop enumerating windows
			D2DxOverlay::EnumHwnd = hwnd;
			return FALSE;
		}

		// If the process ID does not match, continue enumerating windows by returning TRUE.
		return TRUE;
	}

	DWORD WINAPI OverlayThread(LPVOID lpParam)
	{
		INIT_CONSOLE();
		SPE("Overlay Thread Started");
		LOG("Move to Enum Windows");

		// Enumerates all top-level windows on the screen 
		// .. passes each window handle to the callback function EnumWindowsProc
		EnumWindows(EnumWindowsProc, NULL);

		INF("Enum Windows Complete");

		LOG("Moving to Dx Setup");

		// Create the (singleton) instance of the D2DxOverlay
		D2DxOverlay d2;

		// Setup D2D and window resources
		d2.Initialize(D2DxOverlay::EnumHwnd);
		INF("Dx Setup complete, starting render loop");

		// Enter the rendering loop
		while (d2.RenderLoop() && !D2DxOverlay::exit) {}

		LOG("Dx Renderer Exited");

		INF("Freeing Console.");

		// Clean up console resources and exit the thread
		EXIT_CONSOLE();
		return 0;
	}

	void RunOverlayThread(const DirectOverlayCallback callback) {
		D2DxOverlay::DrawLoopCallback = callback;
		CreateThread(nullptr, 0, OverlayThread, nullptr, 0, nullptr);
	}

}