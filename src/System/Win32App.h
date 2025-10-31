#pragma once

#include <windows.h>
#include "Render/D3D/D3DInstance.h"
#include "Render/D3D/SwapChain.h"

namespace GameLogic
{
class Game;
}

class Win32App
{
public:
    using DisplayMode = std::pair<UINT, UINT>;

public:
    static int Run(HINSTANCE hInstance, LPSTR lpszArgument, int nCmdShow);
    static void Shutdown();
    static void ShowMessage(const char* msg, const char* title);
    
    static const std::vector<DisplayMode>& DisplayModeList() { return m_displayModeList; }

    static void SetDisplayMode(UINT m);
    static void Resize(UINT m);

    static bool IsFullscreen() { return m_fullscreen; }
    static void ToggleFullscreen();

    static inline HWND GetWindowHandle() { return m_hwnd; }
    static inline Render::FrameBuffer& GetFrameBuffer() { return m_swapChain.getFrameBuffer(); }

    static void SyncTime();

private:
    static void EnumDisplayModes();
    static void GameTick();

    static LRESULT CALLBACK WindowProcedure(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam);

private:
    static HWND m_hwnd;
    static Render::SwapChain m_swapChain;

    static std::vector<DisplayMode> m_displayModeList;
    static UINT m_displayMode;
    
    static int m_xcenter;
    static int m_ycenter;
    static bool m_fullscreen;
    static bool m_active;

    static DWORD m_time;
    static float m_dt;

    static GameLogic::Game* m_game;
    
    static constexpr UINT WindowStyle = WS_OVERLAPPEDWINDOW;
};

using App = Win32App;