#pragma once

namespace windows_utils
{
    bool hwnd_rel_curpos(POINT& point)
    {
        if (const HWND foreground_window = GetForegroundWindow();
            foreground_window == nullptr || !GetCursorPos(&point) || !ScreenToClient(foreground_window, &point)) {
            return false;
        }
        return true;
    }
    bool abs_curpos(POINT& point)
    {
        if (!GetCursorPos(&point)) {
            return false;
        }
        return true;
    }
}