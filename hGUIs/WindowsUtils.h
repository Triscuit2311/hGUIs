#pragma once

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

}