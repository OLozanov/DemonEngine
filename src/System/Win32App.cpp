#include <tchar.h>
#include <windows.h>
#include <windowsx.h>

#include "Win32App.h"
#include "Render/SceneManager.h"
#include "UI/UiLayer.h"
#include "Game/Game.h"
#include "Resources/Resources.h"

#include "System/Timer.h"

#include <set>

wchar_t szClassName[] = L"GameWnd";

HWND Win32App::m_hwnd;
Render::SwapChain Win32App::m_swapChain;

int Win32App::m_xcenter;
int Win32App::m_ycenter;
bool Win32App::m_fullscreen = false;
bool Win32App::m_active = false;

std::vector<Win32App::DisplayMode> Win32App::m_displayModeList;
UINT Win32App::m_displayMode = 7;

DWORD Win32App::m_time = 0;
float Win32App::m_dt = 1.0f / 60.0f;

GameLogic::Game* Win32App::m_game = nullptr;

int Win32App::Run(HINSTANCE hInstance, LPSTR lpszArgument, int nCmdShow)
{
    WNDCLASSEX wincl;

    wincl.hInstance = hInstance;
    wincl.lpszClassName = szClassName;
    wincl.lpfnWndProc = WindowProcedure;      /* This function is called by windows */
    wincl.style = CS_DBLCLKS;                 /* Catch double-clicks */
    wincl.cbSize = sizeof(WNDCLASSEX);
    wincl.hIcon = LoadIcon(NULL, IDI_APPLICATION);
    wincl.hIconSm = LoadIcon(NULL, IDI_APPLICATION);
    wincl.hCursor = LoadCursor(NULL, IDC_ARROW);
    wincl.lpszMenuName = NULL;                 /* No menu */
    wincl.cbClsExtra = 0;                      /* No extra bytes after the window class */
    wincl.cbWndExtra = 0;                      /* structure or the window instance */
    wincl.hbrBackground = (HBRUSH)COLOR_BACKGROUND;

    if (!RegisterClassEx(&wincl)) return 0;

    RECT winRect = { 0, 0, 1280, 720 };
    AdjustWindowRect(&winRect, WindowStyle, FALSE);

    int width = winRect.right - winRect.left;
    int height = winRect.bottom - winRect.top;

    m_hwnd = CreateWindowEx(
        0,                   /* Extended possibilites for variation */
        szClassName,         /* Classname */
        L"Game",             /* Title Text */
        WindowStyle,         /* default window */
        CW_USEDEFAULT,       /* Windows decides the position */
        CW_USEDEFAULT,       /* where the window ends up on the screen */
        width,               /* The programs width */
        height,              /* and height in pixels */
        HWND_DESKTOP,        /* The window is a child-window to desktop */
        NULL,                /* No menu */
        hInstance,           /* Program Instance handler */
        nullptr              /* No Window Creation data */
    );

    ShowWindow(m_hwnd, nCmdShow);

    EnumDisplayModes();

#ifdef _DEBUG
    if (AttachConsole(ATTACH_PARENT_PROCESS))
    {   
        FILE* fp_out;
        freopen_s(&fp_out, "CONOUT$", "w", stdout);
    }
#endif

    //Initialization
    RECT clientRect = {};
    GetClientRect(m_hwnd, &clientRect);
    width = clientRect.right - clientRect.left;
    height = clientRect.bottom - clientRect.top;

    Render::D3DInstance& d3dInstance = Render::D3DInstance::GetInstance();

    m_swapChain = d3dInstance.createSwapChain(m_hwnd);

    Render::SceneManager& sceneManager = Render::SceneManager::GetInstance();
    UI::UiLayer& uiLayer = UI::UiLayer::GetInstance();

    sceneManager.init(width, height);
    uiLayer.init(width, height);

    m_game = new GameLogic::Game();
    m_game->onScreenResize(width, height);

    SetWindowLongPtr(m_hwnd, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(m_game));

    ShowCursor(false);

    m_game->loadSettings();
    m_game->processCommandLine(lpszArgument);

    //Main loop
    MSG message = {};

    m_time = timeGetTime();

    while (message.message != WM_QUIT)
    {
        if (PeekMessage(&message, NULL, 0, 0, PM_REMOVE))
        {
            TranslateMessage(&message);
            DispatchMessage(&message);
        }
    }

    m_game->saveSettings();

    return message.wParam;
}

void Win32App::GameTick()
{
    if (!m_active) return;
    if (!m_game) return;

    Render::SceneManager& sceneManager = Render::SceneManager::GetInstance();
    UI::UiLayer& uiLayer = UI::UiLayer::GetInstance();

    DWORD curtime = timeGetTime();

    if (curtime > m_time)
    {
        static constexpr float w = 1.0f / 30.0f;

        float delta = (curtime - m_time) / 1000.0f;
        m_dt = delta * w + m_dt * (1.0f - w);

        m_time = curtime;
    }

    POINT cursorPos;

    GetCursorPos(&cursorPos);
    m_game->onMouseMove(cursorPos.x - m_xcenter, cursorPos.y - m_ycenter);
    SetCursorPos(m_xcenter, m_ycenter);

    m_game->update(m_dt);
    uiLayer.update(m_dt);
    ResourceManager::AnimateMaps(m_dt);
    Timer::UpdateTimers(m_dt);

    sceneManager.display();
    uiLayer.display();
    m_swapChain.present();
}

void Win32App::Shutdown()
{
    SetWindowLongPtr(m_hwnd, GWLP_USERDATA, 0);
    PostQuitMessage(0);
}

void Win32App::ShowMessage(const char* msg, const char * title)
{
    MessageBoxA(m_hwnd, msg, title, MB_OK);
}

void Win32App::EnumDisplayModes()
{
    // TODO: For better support of multi-monitor configurations it probably worth to track current monitor
    /*HMONITOR monitor = MonitorFromWindow(m_hwnd, MONITOR_DEFAULTTONEAREST);

    std::wstring mname;

    if (monitor != NULL)
    {
        MONITORINFOEX monitorInfo;
        monitorInfo.cbSize = sizeof(MONITORINFOEX);

        if (GetMonitorInfo(monitor, &monitorInfo))
            mname = monitorInfo.szDevice;
    }*/

    std::set<DisplayMode> modes;
    m_displayModeList.clear();
    
    DEVMODE devMode = {};
    devMode.dmSize = sizeof(devMode);
    
    size_t modeNum = 0;
  
    while (EnumDisplaySettingsEx(NULL, modeNum++, &devMode, 0))
    {
        modes.insert({ devMode.dmPelsWidth, devMode.dmPelsHeight });
        
        ZeroMemory(&devMode, sizeof(devMode));
        devMode.dmSize = sizeof(devMode);
    }

    for (const auto& mode : modes) m_displayModeList.emplace_back(mode);
}

void Win32App::SetDisplayMode(UINT m)
{
    const auto& mode = m_displayModeList[m];

    DEVMODE devmode;

    devmode.dmSize = sizeof(DEVMODE);
    devmode.dmPelsWidth = mode.first;
    devmode.dmPelsHeight = mode.second;
    devmode.dmFields = DM_PELSWIDTH | DM_PELSHEIGHT;

    ChangeDisplaySettings(&devmode, CDS_FULLSCREEN);
}

void Win32App::Resize(UINT m)
{
    m_displayMode = m;

    const auto& mode = m_displayModeList[m];

    RECT rect = { 0, 0 , mode.first, mode.second };

    if (!m_fullscreen) AdjustWindowRect(&rect, WindowStyle, FALSE);
    
    int w = rect.right - rect.left;
    int h = rect.bottom - rect.top;

    if (m_fullscreen) SetDisplayMode(m);

    int x = m_fullscreen ? 0 : (GetSystemMetrics(SM_CXSCREEN) - w) / 2;
    int y = m_fullscreen ? 0 : (GetSystemMetrics(SM_CYSCREEN) - h) / 2;

    SetWindowPos(m_hwnd, HWND_TOP, x, y, w, h, 0);
}

void Win32App::ToggleFullscreen()
{
    m_fullscreen = !m_fullscreen;

    if (m_fullscreen)
    {
        SetWindowLong(m_hwnd, GWL_STYLE, WS_OVERLAPPED);
        ShowWindow(m_hwnd, SW_NORMAL);
    }
    else
    {
        SetWindowLong(m_hwnd, GWL_STYLE, WindowStyle);
        ShowWindow(m_hwnd, SW_NORMAL);

        ChangeDisplaySettings(NULL, 0);
    }

    Resize(m_displayMode);

    m_game->onSwichFullscreen(m_fullscreen);
}

void Win32App::SyncTime()
{
    m_time = timeGetTime();
}

LRESULT CALLBACK Win32App::WindowProcedure(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    GameLogic::Game* game = reinterpret_cast<GameLogic::Game*>(GetWindowLongPtr(hwnd, GWLP_USERDATA));

    switch(message)
    {
        case WM_KEYDOWN:
            if(game)
            {
                game->onKeyPress(wParam, true);
                UI::UiLayer::GetInstance().onKeyPress(wParam, true);
            }
        break;

        case WM_KEYUP:
            if (game)
            {
                game->onKeyPress(wParam, false);
            }
        break;

        case WM_LBUTTONDOWN:
            if (game)
            {
                game->onMouseButton(0, true);
            }
        break;

        case WM_LBUTTONUP:
            if (game)
            {
                game->onMouseButton(0, false);
            }
        break;

        case WM_RBUTTONDOWN:
            if (game)
            {
                game->onMouseButton(1, true);
            }
        break;

        case WM_RBUTTONUP:
            if (game)
            {
                game->onMouseButton(1, false);
            }
        break;

        case WM_MOUSEWHEEL:
            if (game)
            {
                WORD keys = GET_KEYSTATE_WPARAM(wParam);
                short delta = GET_WHEEL_DELTA_WPARAM(wParam);

                game->onMouseWheel(delta);
            }
        break;

        case WM_CHAR:
            UI::UiLayer::GetInstance().onCharInput(wParam);
        break;

        case WM_SYSKEYDOWN:
        
        // Alt+Enter
        if ((wParam == VK_RETURN) && (lParam & (1 << 29)))
        {
            ToggleFullscreen();
            return 0;
        }
        break;

        case WM_SIZE:
        {
            RECT windowRect = {};
            GetWindowRect(hwnd, &windowRect);

            m_xcenter = windowRect.left + int(LOWORD(lParam) / 2);
            m_ycenter = windowRect.top + int(HIWORD(lParam) / 2);

            RECT clientRect = {};
            GetClientRect(hwnd, &clientRect);
            if (wParam != SIZE_MINIMIZED && game)
            {
                int width = clientRect.right - clientRect.left;
                int height = clientRect.bottom - clientRect.top;

                m_swapChain.resize(width, height);
                Render::SceneManager::GetInstance().resize(width, height);
                UI::UiLayer::GetInstance().resize(width, height);

                game->onScreenResize(width, height);
            }

            m_xcenter = windowRect.left + (windowRect.right - windowRect.left) / 2;
            m_ycenter = windowRect.top + (windowRect.bottom - windowRect.top) / 2;
            SetCursorPos(m_xcenter, m_ycenter);
        }
        break;

        case WM_ACTIVATE:
        {
            m_active = LOWORD(wParam) == WA_INACTIVE ? false : true;

            if (m_fullscreen)
            {
                if (m_active)
                {
                    SetWindowLong(m_hwnd, GWL_STYLE, WS_OVERLAPPED);
                    ShowWindow(m_hwnd, SW_NORMAL);

                    SetDisplayMode(m_displayMode);
                }
                else
                {
                    //SetWindowLong(m_hwnd, GWL_STYLE, WindowStyle);
                    ShowWindow(m_hwnd, SW_MINIMIZE);

                    ChangeDisplaySettings(NULL, 0);
                }
            }

            if (m_active) SyncTime();
        }
        break;

        case WM_PAINT:
            GameTick();
        break;

        case WM_DESTROY:
            PostQuitMessage(0);
        break;

        default:
            return DefWindowProc(hwnd, message, wParam, lParam);
    }

    return 0;
}