#ifndef PCH_H
#define PCH_H

#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif

#include <windows.h>
#include <complex.h>
#include <string>
#include <fstream>
#include <comdef.h>
#include <iostream>
#include <ctime>
#include <cstdio>
#include <vector>
#include <map>
#include <queue>
#include <unordered_map>
#include <ranges>
#include <filesystem>
#include <any>
#include <functional>
#include <mutex>
#include <thread>
#include <variant>
#include <any>
#include <utility>
#include <format>


#include <d2d1.h>
#include <dwrite.h>
#include <dwmapi.h>
#include <wincodec.h>

#pragma comment(lib, "d2d1.lib")
#pragma comment(lib, "dwmapi.lib")
#pragma comment(lib, "dwrite.lib")
#pragma comment(lib, "Windowscodecs.lib")

#define DIRECTINPUT_VERSION 0x0800
#include <dinput.h>

#endif //PCH_H
