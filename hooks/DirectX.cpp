#include "pch-il2cpp.h"
#include "DirectX.h"
#include "Renderer.hpp"
#include "imgui/imgui.h"
#include "imgui/imgui_internal.h"
#include "imgui/imgui_impl_dx11.h"
#include "imgui/imgui_impl_win32.h"
#define STB_TRUETYPE_IMPLEMENTATION
#include "imgui/imstb_truetype.h"
#include "keybinds.h"
#include "menu.hpp"
#include "radar.hpp"
#include "replay.hpp"
#include "esp.hpp"
#include "state.hpp"
#include "theme.hpp"
#include <mutex>
#include "logger.h"
#include "resource_data.h"
#include "game.h"
#include "console.hpp"
#include "profiler.h"

#include <future>
using namespace app;

extern LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

HWND DirectX::window;
ID3D11Device* pDevice = NULL;
ID3D11DeviceContext* pContext = NULL;
ID3D11RenderTargetView* pRenderTargetView = NULL;
D3D_PRESENT_FUNCTION oPresent = nullptr;
WNDPROC oWndProc;

HANDLE DirectX::hRenderSemaphore;
constexpr DWORD MAX_RENDER_THREAD_COUNT = 5; //Should be overkill for our purposes

std::vector<MapTexture> maps = std::vector<MapTexture>();
std::unordered_map<ICON_TYPES, IconTexture> icons;

typedef struct Cache
{
	ImGuiWindow* Window = nullptr;  //Window instance
	ImVec2       Winsize; //Size of the window
} cache_t;

static cache_t s_Cache;

ImVec2 DirectX::GetWindowSize()
{
    if (Screen_get_fullScreen(nullptr))
    {
        RECT rect;
        GetWindowRect(window, &rect);

        return { (float)(rect.right - rect.left),  (float)(rect.bottom - rect.top) };
    }

    return { (float)Screen_get_width(nullptr), (float)Screen_get_height(nullptr) };

}

static bool CanDrawEsp()
{
	return (!MenuState.PanicMode && IsInGame() || IsInLobby()) && MenuState.ShowEsp && (!MenuState.InMeeting || !state.hideEsp_During_Meetings);
}

static bool CanDrawRadar()
{
	return !MenuState.PanicMode && IsInGame() && MenuState.ShowRadar && (!MenuState.InMeeting || !state.hideRadar_During_Meetings);
}

static bool CanDrawReplay()
{
    return !MenuState.PanicMode && IsInGame() && MenuState.ShowReplay;
}

LRESULT __stdcall dWndProc(const HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
    if (!MenuState.ImGuiInitialized)
        return CallWindowProc(oWndProc, hWnd, uMsg, wParam, lParam);

    if (uMsg == WM_DPICHANGED && MenuState.AdjustByDPI) {
        float dpi = HIWORD(wParam);
        MenuState.dpiScale = dpi / 96.0f;
        MenuState.dpiChanged = true;
        STREAM_DEBUG("DPI Scale: " << MenuState.dpiScale);
    }

    if (uMsg == WM_SIZE) {
        // RenderTarget needs to be released because the resolution has changed 
        WaitForSingleObject(DirectX::hRenderSemaphore, INFINITE);
        if (pRenderTargetView) {
            pRenderTargetView->Release();
            pRenderTargetView = nullptr;
        }
        ReleaseSemaphore(DirectX::hRenderSemaphore, 1, NULL);
    }

    if (ImGui_ImplWin32_WndProcHandler(hWnd, uMsg, wParam, lParam))
        return true;

    KeyBinds::WndProc(uMsg, wParam, lParam);

    if (!MenuState.PanicMode && !MenuState.KeybindsBeingEdited && (!MenuState.ChatFocused || MenuState.KeybindsWhileChatting) /*disable keybinds when chatting*/) {
        if (KeyBinds::IsKeyPressed(MenuState.KeyBinds.Toggle_Menu)) MenuState.ShowMenu = !MenuState.ShowMenu;
        if (KeyBinds::IsKeyPressed(MenuState.KeyBinds.Toggle_Radar)) MenuState.ShowRadar = !MenuState.ShowRadar;
        if (KeyBinds::IsKeyPressed(MenuState.KeyBinds.Toggle_Console)) MenuState.ShowConsole = !MenuState.ShowConsole;
        if (KeyBinds::IsKeyPressed(MenuState.KeyBinds.Repair_Sabotage) && IsInGame()) RepairSabotage(*Game::pLocalPlayer);
        if (KeyBinds::IsKeyPressed(MenuState.KeyBinds.Toggle_Noclip) && (IsInGame() || IsInLobby())) { MenuState.NoClip = !MenuState.NoClip; state.hotkeyNoClip = true; }
        if (KeyBinds::IsKeyPressed(MenuState.KeyBinds.Toggle_Autokill) && (IsInGame() || IsInLobby())) MenuState.AutoKill = !MenuState.AutoKill;
        if (KeyBinds::IsKeyPressed(MenuState.KeyBinds.Close_All_Doors) && IsInGame()) MenuState.CloseAllDoors = true;
        if (KeyBinds::IsKeyPressed(MenuState.KeyBinds.Toggle_Zoom) && (IsInGame() || IsInLobby())) { MenuState.EnableZoom = !MenuState.EnableZoom; if (!MenuState.EnableZoom) RefreshChat(); }
        if (KeyBinds::IsKeyPressed(MenuState.KeyBinds.Toggle_Freecam) && (IsInGame() || IsInLobby())) {
            MenuState.FreeCam = !MenuState.FreeCam;
            MenuState.playerToFollow = {};
        }
        if (KeyBinds::IsKeyPressed(MenuState.KeyBinds.Close_Current_Room_Door) && IsInGame()) MenuState.rpcQueue.push(new RpcCloseDoorsOfType(GetSystemTypes(GetTrueAdjustedPosition(*Game::pLocalPlayer)), false));
        if (KeyBinds::IsKeyPressed(MenuState.KeyBinds.Toggle_Replay)) MenuState.ShowReplay = !MenuState.ShowReplay;
        if (KeyBinds::IsKeyPressed(MenuState.KeyBinds.Toggle_ChatAlwaysActive) && (IsInGame() || IsInLobby())) MenuState.ChatAlwaysActive = !MenuState.ChatAlwaysActive;
        if (KeyBinds::IsKeyPressed(MenuState.KeyBinds.Toggle_ReadGhostMessages) && (IsInGame() || IsInLobby())) MenuState.ReadGhostMessages = !MenuState.ReadGhostMessages;
        if (KeyBinds::IsKeyPressed(MenuState.KeyBinds.Toggle_Hud) && (IsInGame() || IsInLobby())) MenuState.DisableHud = !MenuState.DisableHud;
        if (KeyBinds::IsKeyPressed(MenuState.KeyBinds.Reset_Appearance) && (IsInGame() || IsInLobby())) ControlAppearance(false);
        if (KeyBinds::IsKeyPressed(MenuState.KeyBinds.Randomize_Appearance)) ControlAppearance(true);
        if (KeyBinds::IsKeyPressed(MenuState.KeyBinds.Complete_Tasks) && IsInGame()) CompleteAllTasks();
        if (MenuState.EnableZoom && (IsInGame() || IsInLobby())) {
            if (ImGui::GetIO().MouseWheel < 0.f)  MenuState.CameraHeight += 0.1f;
            if (ImGui::GetIO().MouseWheel > 0.f) {
                MenuState.CameraHeight -= 0.1f;
                if (MenuState.CameraHeight < 1.f) MenuState.CameraHeight = 1.f;
            }
        }
        if ((ImGui::IsKeyDown(VK_SHIFT) || ImGui::IsKeyDown(VK_LSHIFT) || ImGui::IsKeyDown(VK_RSHIFT)) && MenuState.FreeCam && (IsInGame() || IsInLobby())) {
            if (ImGui::GetIO().MouseWheel < 0.f ) MenuState.FreeCamSpeed += 0.05f;
            if (ImGui::GetIO().MouseWheel > 0.f && MenuState.CameraHeight - 0.05f >= 0.05f) MenuState.FreeCamSpeed -= 0.05f;
        }
    }
    if (KeyBinds::IsKeyPressed(MenuState.KeyBinds.Toggle_Sicko)) MenuState.PanicMode = !MenuState.PanicMode;
    return CallWindowProc(oWndProc, hWnd, uMsg, wParam, lParam);
}

bool ImGuiInitialization(IDXGISwapChain* pSwapChain) {
    if ((pDevice != NULL) || (SUCCEEDED(pSwapChain->GetDevice(__uuidof(ID3D11Device), (void**)&pDevice)))) {
        pDevice->GetImmediateContext(&pContext);
        DXGI_SWAP_CHAIN_DESC sd;
        pSwapChain->GetDesc(&sd);
        DirectX::window = sd.OutputWindow;
        ID3D11Texture2D* pBackBuffer = NULL;
        pSwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (LPVOID*)&pBackBuffer);
        if (!pBackBuffer)
            return false;
        pDevice->CreateRenderTargetView(pBackBuffer, NULL, &pRenderTargetView);
        pBackBuffer->Release();
        oWndProc = (WNDPROC)SetWindowLongPtr(DirectX::window, GWLP_WNDPROC, (LONG_PTR)dWndProc);
        /*if (MenuState.AdjustByDPI) {
            MenuState.dpiScale = ImGui_ImplWin32_GetDpiScaleForHwnd(DirectX::window);
        }
        else {
            MenuState.dpiScale = 1.0f;
        }
        MenuState.dpiChanged = true;*/

        ImGui::CreateContext();
        ImGuiIO& io = ImGui::GetIO();
        io.ConfigFlags = ImGuiConfigFlags_NoMouseCursorChange;
        ImGui_ImplWin32_Init(DirectX::window);
        ImGui_ImplDX11_Init(pDevice, pContext);

        maps.push_back({ D3D11Image(Resource(IDB_PNG1), pDevice), 277.F, 77.F, 11.5F });
        maps.push_back({ D3D11Image(Resource(IDB_PNG2), pDevice), 115.F, 240.F, 9.25F });
        maps.push_back({ D3D11Image(Resource(IDB_PNG3), pDevice), 8.F, 21.F, 10.F });
        maps.push_back({ D3D11Image(Resource(IDB_PNG4), pDevice), 162.F, 107.F, 6.F });
        maps.push_back({ D3D11Image(Resource(IDB_PNG15), pDevice), 237.F, 140.F, 8.5F });

        icons.insert({ ICON_TYPES::VENT_IN, { D3D11Image(Resource(IDB_PNG5), pDevice), 0.02f }});
        icons.insert({ ICON_TYPES::VENT_OUT, { D3D11Image(Resource(IDB_PNG6), pDevice), 0.02f }});
        icons.insert({ ICON_TYPES::KILL, { D3D11Image(Resource(IDB_PNG7), pDevice), 0.02f } });
        icons.insert({ ICON_TYPES::REPORT, { D3D11Image(Resource(IDB_PNG8), pDevice), 0.02f } });
        icons.insert({ ICON_TYPES::TASK, { D3D11Image(Resource(IDB_PNG9), pDevice), 0.02f } });
        icons.insert({ ICON_TYPES::PLAYER, { D3D11Image(Resource(IDB_PNG10), pDevice), 0.02f } });
        icons.insert({ ICON_TYPES::CROSS, { D3D11Image(Resource(IDB_PNG11), pDevice), 0.02f } });
        icons.insert({ ICON_TYPES::DEAD, { D3D11Image(Resource(IDB_PNG12), pDevice), 0.02f } });
        icons.insert({ ICON_TYPES::PLAY, { D3D11Image(Resource(IDB_PNG13), pDevice), 0.55f } });
        icons.insert({ ICON_TYPES::PAUSE, { D3D11Image(Resource(IDB_PNG14), pDevice), 0.55f } });
        icons.insert({ ICON_TYPES::PLAYERVISOR, { D3D11Image(Resource(IDB_PNG16), pDevice), 0.02f } });

        DirectX::hRenderSemaphore = CreateSemaphore(
            NULL,                                 // default security attributes
            MAX_RENDER_THREAD_COUNT,              // initial count
            MAX_RENDER_THREAD_COUNT,              // maximum count
            NULL);                                // unnamed semaphore);
        return true;
    }
    
    return false;
}

static void RebuildFont() {
    ImGuiIO& io = ImGui::GetIO();
    io.Fonts->Clear();
    io.Fonts->AddFontFromFileTTF("C:\\Windows\\Fonts\\Arial.ttf", 14 * MenuState.dpiScale, nullptr, io.Fonts->GetGlyphRangesCyrillic());
    do {
        const ImWchar* glyph_ranges;
        wchar_t locale[LOCALE_NAME_MAX_LENGTH] = { 0 };
        ::GetUserDefaultLocaleName(locale, LOCALE_NAME_MAX_LENGTH);
        if (!_wcsnicmp(locale, L"zh-", 3)) {
            // China
            glyph_ranges = io.Fonts->GetGlyphRangesChineseSimplifiedCommon();
        }
        else if (!_wcsnicmp(locale, L"ja-", 3)) {
            // Japan
            glyph_ranges = io.Fonts->GetGlyphRangesJapanese();
        }
        else if (!_wcsnicmp(locale, L"ko-", 3)) {
            // Korea
            glyph_ranges = io.Fonts->GetGlyphRangesKorean();
        }
        else {
            break;
        }
        NONCLIENTMETRICSW nm = { sizeof(NONCLIENTMETRICSW) };
        if (!::SystemParametersInfoW(SPI_GETNONCLIENTMETRICS, nm.cbSize, &nm, 0))
            break;
        auto hMessageFont = ::CreateFontIndirectW(&nm.lfMessageFont);
        if (!hMessageFont)
            break;
        void* fontData = nullptr;
        HDC hdc = ::GetDC(DirectX::window);
        auto hDefaultFont = ::SelectObject(hdc, hMessageFont);
        do {
            // Try to get TTC first
            DWORD dwTableTag = 0x66637474;/*TTCTag*/
            DWORD dwSize = ::GetFontData(hdc, dwTableTag, 0, 0, 0);
            if (dwSize == GDI_ERROR) {
                // Maybe TTF
                dwTableTag = 0;
                dwSize = ::GetFontData(hdc, 0, 0, 0, 0);
                if (dwSize == GDI_ERROR)
                    break;
            }
            fontData = IM_ALLOC(dwSize);
            if (!fontData)
                break;
            if (::GetFontData(hdc, dwTableTag, 0, fontData, dwSize) != dwSize)
                break;
            ImFontConfig config;
            if (dwTableTag != 0) {
                // Get index of font within TTC
                DWORD dwTTFSize = ::GetFontData(hdc, 0, 0, 0, 0);
                if (dwTTFSize < dwSize) {
                    auto offsetTTF = dwSize - dwTTFSize;
                    int n = stbtt_GetNumberOfFonts((unsigned char*)fontData);
                    for (int index = 0; index<n; index++) {
                        if (offsetTTF == ttULONG((unsigned char*)fontData + 12 + index * 4)) {
                            config.FontNo = index;
                            break;
                        }
                    }
                }
            }
            config.MergeMode = true;
            io.Fonts->AddFontFromMemoryTTF(fontData, dwSize, 14 * MenuState.dpiScale, &config, glyph_ranges);
            fontData = nullptr;
        } while (0);
        if (fontData)
            IM_FREE(fontData);
        ::SelectObject(hdc, hDefaultFont);
        ::DeleteObject(hMessageFont);
        ::ReleaseDC(DirectX::window, hdc);
    } while (0);
    io.Fonts->Build();
}

std::once_flag init_d3d;
HRESULT __stdcall dPresent(IDXGISwapChain* __this, UINT SyncInterval, UINT Flags) {
    std::call_once(init_d3d, [&] {
        if (SUCCEEDED(__this->GetDevice(__uuidof(ID3D11Device), (void**)&pDevice)))
        {
            pDevice->GetImmediateContext(&pContext);
        }
    });
	if (!MenuState.ImGuiInitialized) {
        if (ImGuiInitialization(__this)) {
            ImVec2 size = DirectX::GetWindowSize();
            MenuState.ImGuiInitialized = true;
            STREAM_DEBUG("ImGui Initialized successfully!");
            STREAM_DEBUG("Fullscreen: " << Screen_get_fullScreen(nullptr));
            STREAM_DEBUG("Unity Window Resolution: " << +Screen_get_width(nullptr) << "x" << +Screen_get_height(nullptr));
            STREAM_DEBUG("DirectX Window Size: " << +size.x << "x" << +size.y);
        } else {
            ReleaseSemaphore(DirectX::hRenderSemaphore, 1, NULL);
            return oPresent(__this, SyncInterval, Flags);
        }
    }

    if (!Profiler::HasInitialized)
    {
        Profiler::InitProfiling();
    }

    WaitForSingleObject(DirectX::hRenderSemaphore, INFINITE);

    // resolution changed
    if (!pRenderTargetView) {
        ID3D11Texture2D* pBackBuffer = nullptr;
        __this->GetBuffer(0, __uuidof(ID3D11Texture2D), (LPVOID*)&pBackBuffer);
        assert(pBackBuffer);
        pDevice->CreateRenderTargetView(pBackBuffer, nullptr, &pRenderTargetView);
        pBackBuffer->Release();

        ImVec2 size = DirectX::GetWindowSize();
        STREAM_DEBUG("Unity Window Resolution: " << +Screen_get_width(nullptr) << "x" << +Screen_get_height(nullptr));
        STREAM_DEBUG("DirectX Window Size: " << +size.x << "x" << +size.y);
    }

    if (MenuState.dpiChanged) {
        MenuState.dpiChanged = false;
        ImGui_ImplDX11_InvalidateDeviceObjects();
        RebuildFont();
    }

    il2cpp_gc_disable();

    ApplyTheme();
    ImGui_ImplDX11_NewFrame();
    ImGui_ImplWin32_NewFrame();
    ImGui::NewFrame();

    if (!MenuState.PanicMode && MenuState.ShowMenu)
    {
        ImGuiRenderer::Submit([]() { Menu::Render(); });
    }

    if (!MenuState.PanicMode && MenuState.ShowConsole)
    {
        ImGuiRenderer::Submit([]() { ConsoleGui::Render(); });
    }

    if (CanDrawEsp())
	{
		ImGuiRenderer::Submit([&]()
		{
			//Push ImGui flags
			ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.f * MenuState.dpiScale);
			ImGui::PushStyleColor(ImGuiCol_WindowBg, { 0.0f, 0.0f, 0.0f, 0.0f });

			//Setup BackBuffer
			ImGui::Begin("BackBuffer", reinterpret_cast<bool*>(true),
				ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoInputs | ImGuiWindowFlags_NoScrollbar);

			s_Cache.Winsize = DirectX::GetWindowSize();
			s_Cache.Window = ImGui::GetCurrentWindow();

			//Set window properties
			ImGui::SetWindowPos({ 0, 0 }, ImGuiCond_Always);
			ImGui::SetWindowSize(s_Cache.Winsize, ImGuiCond_Always);

			Esp::Render();

			s_Cache.Window->DrawList->PushClipRectFullScreen();

			ImGui::PopStyleColor();
			ImGui::PopStyleVar();
			ImGui::End();
		});
	}

	if (CanDrawRadar())
	{
		ImGuiRenderer::Submit([]() { Radar::Render(); });
	}

    if (CanDrawReplay())
    {
        ImGuiRenderer::Submit([]() { Replay::Render(); });
    }

    // Render in a separate thread
	std::async(std::launch::async, ImGuiRenderer::ExecuteQueue).wait();

    ImGui::EndFrame();
    ImGui::Render();

    pContext->OMSetRenderTargets(1, &pRenderTargetView, NULL);
    ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());

    il2cpp_gc_enable();

    HRESULT result = oPresent(__this, SyncInterval, Flags);

    ReleaseSemaphore(DirectX::hRenderSemaphore, 1, NULL);

    return result;
}

void DirectX::Shutdown() {
    assert(hRenderSemaphore != NULL); //Initialization is now in a hook, so we might as well guard against this
    for (uint8_t i = 0; i < MAX_RENDER_THREAD_COUNT; i++) //This ugly little hack means we use up all the render queues so we can end everything
    {
        assert(WaitForSingleObject(hRenderSemaphore, INFINITE) == WAIT_OBJECT_0); //Since this is only used on debug builds, we'll leave this for now
    }
    oWndProc = (WNDPROC)SetWindowLongPtr(window, GWLP_WNDPROC, (LONG_PTR)oWndProc);
    ImGui_ImplDX11_Shutdown();
    ImGui_ImplWin32_Shutdown();
    ImGui::DestroyContext();
    CloseHandle(hRenderSemaphore);
}



