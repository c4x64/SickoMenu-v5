#include "pch-il2cpp.h"
#include "main.h"
#include "il2cpp-init.h"
#include "il2cpp-helpers.h" // ADDED THIS: It replaces the need for raw function includes
#include <VersionHelpers.h>
#include "crc32.h"
#include <shellapi.h>
#include <iostream>
#include "game.h"
#include "_hooks.h"
#include "logger.h"
#include "state.hpp"
#include "version.h"
#include <fstream>
#include <sstream>

using namespace app;

HMODULE hModule;
HANDLE hUnloadEvent;

std::string GetCRC32(std::filesystem::path filePath) {
    CRC32 crc32;
    char buffer[4096] = { 0 };
    std::ifstream fin(filePath, std::ifstream::binary);
    while (!fin.eof()) {
        fin.read(&buffer[0], 4096);
        auto readSize = fin.gcount();
        crc32.add(&buffer[0], (size_t)readSize);
    }
    return crc32.getHash();
}

bool GameVersionCheck() {
    auto modulePath = getModulePath(NULL);
    auto gameAssembly = modulePath.parent_path() / "GameAssembly.dll";

    if (!std::filesystem::exists(gameAssembly)) {
        SickoLog.Error("GameAssembly.dll was not found");
        MessageBox(NULL, L"Unable to locate GameAssembly.dll", L"SickoMenu", MB_OK | MB_ICONERROR | MB_SYSTEMMODAL);
        return false;
    }
    return true;
}

#define GAME_STATIC_POINTER(f,c,m) \
    do \
    { \
        if (!cctor_finished(c##__TypeInfo->_0.klass)) { \
            if (MessageBox(NULL, \
                L"SickoMenu does not support this Among Us version!\n\nClick OK to exit.", \
                L"SickoMenu", MB_ICONINFORMATION)) { \
                ExitProcess(0); \
            } \
        }; \
        f = &(c##__TypeInfo->static_fields->m); \
    } while (0);

void Run(LPVOID lpParam) {
#if _DEBUG
    new_console();
#endif
    SickoLog.Create();
    if (!GameVersionCheck()) {
        FreeLibraryAndExitThread((HMODULE)lpParam, 0);
        return;
    }
    hModule = (HMODULE)lpParam;
    MenuState.lol = getModulePath(hModule).filename().string();
    
    init_il2cpp();
    
    MenuState.Load();
    ScopedThreadAttacher managedThreadAttached;

    LOG_INFO("SickoMenu Initialized");

#if _DEBUG
    hUnloadEvent = CreateEvent(NULL, FALSE, FALSE, NULL);

    // DEBUG: Checking if essential class pointers were found by init_il2cpp
    if(!AmongUsClient__TypeInfo) LOG_ERROR("Unable to locate AmongUsClient__TypeInfo");
    if(!PlayerControl__TypeInfo) LOG_ERROR("Unable to locate PlayerControl__TypeInfo");
    
    auto domain = il2cpp_domain_get();
#endif

    GAME_STATIC_POINTER(Game::pAmongUsClient, app::AmongUsClient, Instance);
    GAME_STATIC_POINTER(Game::pGameData, app::GameData, Instance);
    GAME_STATIC_POINTER(Game::pAllPlayerControls, app::PlayerControl, AllPlayerControls);
    GAME_STATIC_POINTER(Game::pLocalPlayer, app::PlayerControl, LocalPlayer);
    GAME_STATIC_POINTER(Game::pShipStatus, app::ShipStatus, Instance);
    GAME_STATIC_POINTER(Game::pLobbyBehaviour, app::LobbyBehaviour, Instance);
    
    MenuState.userName = GetPlayerName();

    Game::scanGameFunctions();
    DetourInitilization();

#if _DEBUG
    managedThreadAttached.detach();
    WaitForSingleObject(hUnloadEvent, INFINITE);
    DetourUninitialization();
    fclose(stdout);
    FreeConsole();
    CloseHandle(hUnloadEvent);
    FreeLibraryAndExitThread(hModule, 0);
#endif
}