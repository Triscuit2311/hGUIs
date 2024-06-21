// dllmain.cpp : Defines the entry point for the DLL application.
#include "pch.h"

#include <chrono>
#include <cstdint>
#include <thread>
#include <vector>
#include "../shared/logging.hpp"
#include "../shared/shared_mem.hpp"
#include "../shared/model.hpp"



DWORD WINAPI hThread(LPVOID lpParam)
{
    INIT_CONSOLE();

    std::vector<model::foo> data(6);
    shared_memory_vec<model::foo> shm(L"SharedMemory", 10);


    shared_memory_single<model::menu_settings> menu_shm(L"SHM_MENU_SETTINGS");
    shared_memory_single<model::client_settings> client_shm(L"SHM_CLIENT_SETTINGS");


    for (size_t i = 0; i < data.size(); ++i) {
        data[i].a = (float)i*i;
        data[i].b = (float)i*i;
        data[i].c = (float)i * i + 10;
        data[i].d = (float)i * i + 10;
    }


    int j = 0;
    while (true) {
        j++;
        for (size_t i = 0; i < data.size(); ++i) {
            data[i].a = (float)j+i;
            data[i].b = (float)j+i;
            data[i].c = (float)j+i + 10;
            data[i].d = (float)j+i + 10;
        }


        shm.write(data);


        if (GetAsyncKeyState(VK_NEXT) & 0x01)
        {
            model::g_menu_settings.visuals_to_front = !model::g_menu_settings.visuals_to_front;
            strcpy_s((char*)&model::g_menu_settings.some_str, 128, "HONKYTONKY");
        	
            menu_shm.write(&model::g_menu_settings);
            INF("Updating Menu Setting");
            std::this_thread::sleep_for(std::chrono::milliseconds(250));
        }

        client_shm.read(&model::g_client_settings);
        if(model::g_client_settings.test_print)
        {
            SPE("YEET!");
        	std::this_thread::sleep_for(std::chrono::milliseconds(250));

        }




        std::this_thread::sleep_for(std::chrono::milliseconds(10));
    }


    EXIT_CONSOLE();
	return 0;
}


BOOL WINAPI DllMain(
	HINSTANCE hinstDLL,
	DWORD fdwReason,
	LPVOID lpvReserved)
{
	if (fdwReason == DLL_PROCESS_ATTACH)
	{
		CreateThread(nullptr, 0, hThread, nullptr, 0, nullptr);
	}
	return TRUE;
}
