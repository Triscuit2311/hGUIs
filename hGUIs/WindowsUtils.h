#pragma once
#include "D2xOverlay.h"

namespace windows_utils
{
    inline bool hwnd_rel_curpos(POINT& point)
    {
        if (const HWND foreground_window = GetForegroundWindow();
            foreground_window == nullptr || !GetCursorPos(&point) || !ScreenToClient(foreground_window, &point)) {
            return false;
        }
        return true;
    }
	inline  bool abs_curpos(POINT& point)
    {
        if (!GetCursorPos(&point)) {
            return false;
        }
        return true;
    }

    inline bool is_point_in_rect(const D2D1_RECT_F& r, const LPPOINT& point)
    {
        return point->x > r.left && point->x < r.right && point->y > r.top && point->y < r.bottom;
    }


    inline void get_color_from_curpos(const LPPOINT& point, D2D1_COLOR_F& out)
    {
        HDC hdc = GetDC(Renderer::D2DxOverlay::GetOverlayHWND());
        if (hdc == nullptr) { return; }
        COLORREF c = GetPixel(hdc, point->x, point->y);

        RGBTRIPLE rgb;

        rgb.rgbtRed = GetRValue(c);
        rgb.rgbtGreen = GetGValue(c);
        rgb.rgbtBlue = GetBValue(c);

        out = h_style::theme::colors::rgb_to_color_f(rgb.rgbtRed, rgb.rgbtGreen, rgb.rgbtBlue, out.a);
    }

}