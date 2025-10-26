#include <tchar.h>
#include <windows.h>
#include <windowsx.h>

#include "Win32App.h"
#include "Render/SceneManager.h"
#include "UI/UiLayer.h"
#include "Game/Game.h"
#include "Resources/Resources.h"

#include "System/Timer.h"

wchar_t szClassName[] = L"GameWnd";

bool Win32App::m_run = true;

HWND Win32App::m_hwnd;
Render::SwapChain Win32App::m_swapChain;

int Win32App::m_xcenter;
int Win32App::m_ycenter;
bool Win32App::m_fullscreen;
bool Win32App::m_focus = false;

DWORD Win32App::m_time = 0;

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

    GameLogic::Game game;
    game.onScreenResize(width, height);

    SetWindowLongPtr(m_hwnd, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(&game));

    ShowCursor(false);

    game.loadSettings();
    game.processCommandLine(lpszArgument);

    //Main loop
    MSG message;

    m_time = timeGetTime();

    float dt = 1.0 / 60.0;

    while(m_run)
    {
        DWORD curtime = timeGetTime();

        if (curtime > m_time)
        {
            static constexpr float w = 1.0f / 30.0f;

            float delta = (curtime - m_time) / 1000.0f;
            dt = delta * w + dt * (1.0f - w);

            m_time = curtime;
        }

        while(PeekMessage(&message, NULL, 0, 0, PM_REMOVE))
        {
            if (message.message == WM_QUIT) return message.wParam;

            TranslateMessage(&message);
            DispatchMessage(&message);
        }

        if (m_focus)
        {
            POINT cursorPos;

            GetCursorPos(&cursorPos);
            game.onMouseMove(cursorPos.x - m_xcenter, cursorPos.y - m_ycenter);
            SetCursorPos(m_xcenter, m_ycenter);
        }

        game.update(dt);
        uiLayer.update(dt);
        ResourceManager::AnimateMaps(dt);
        Timer::UpdateTimers(dt);

        sceneManager.display();
        uiLayer.display();
        m_swapChain.present();
    }

    game.saveSettings();

    return message.wParam;
}

void Win32App::Shutdown()
{
    SetWindowLongPtr(m_hwnd, GWLP_USERDATA, 0);
    m_run = false;
}

void Win32App::ShowMessage(const char* msg, const char * title)
{
    MessageBoxA(m_hwnd, msg, title, MB_OK);
}

void Win32App::Resize(int width, int height)
{
    RECT rect = { 0, 0 , width, height };

    if (!m_swapChain.isFullscreen()) AdjustWindowRect(&rect, WindowStyle, FALSE);
    
    int w = rect.right - rect.left;
    int h = rect.bottom - rect.top;

    MoveWindow(m_hwnd, 0, 0, w, h, FALSE);

    /*Render::SceneManager& sceneManager = Render::SceneManager::GetInstance();
    UI::UiLayer& uiLayer = UI::UiLayer::GetInstance();

    m_swapChain.resize(width, height);
    sceneManager.resize(width, height);
    uiLayer.resize(width, height);*/
}

void Win32App::ToggleFullscreen()
{
    m_fullscreen = !m_fullscreen;

    if (m_fullscreen)
    {
        SetWindowLong(m_hwnd, GWL_STYLE, WindowStyle & ~(WS_CAPTION | WS_MAXIMIZEBOX | WS_MINIMIZEBOX | WS_SYSMENU | WS_THICKFRAME));
        ShowWindow(m_hwnd, SW_MAXIMIZE);
    }
    else
    {
        SetWindowLong(m_hwnd, GWL_STYLE, WindowStyle);
        ShowWindow(m_hwnd, SW_NORMAL);
    }

    Render::SceneManager& sm = Render::SceneManager::GetInstance();
    //sm.setFullscreen(m_fullscreen);
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

            /*if (wParam == VK_ESCAPE)
            {
                PostQuitMessage(0);
                SetWindowLongPtr(m_hwnd, GWLP_USERDATA, NULL);
            }*/
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

                //short x = GET_X_LPARAM(lParam);
                //short y = GET_Y_LPARAM(lParam);

                game->onMouseWheel(delta);
            }
        break;

        case WM_CHAR:
            UI::UiLayer::GetInstance().onCharInput(wParam);
        break;

        case WM_SYSKEYDOWN:
        
        // Alt+Enter
        /*if ((wParam == VK_RETURN) && (lParam & (1 << 29)))
        {
            ToggleFullscreen();
            return 0;
        }*/
        break;

        case WM_SIZE:
        {
            RECT windowRect = {};
            GetWindowRect(hwnd, &windowRect);

            m_xcenter = windowRect.left + int(LOWORD(lParam) / 2);
            m_ycenter = windowRect.top + int(HIWORD(lParam) / 2);
        }
        //break;

        // Detect fullscreen switch
        case WM_WINDOWPOSCHANGED:
        {
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

            RECT windowRect = {};
            GetWindowRect(hwnd, &windowRect);

            m_xcenter = windowRect.left + (windowRect.right - windowRect.left) / 2;
            m_ycenter = windowRect.top + (windowRect.bottom - windowRect.top) / 2;
            SetCursorPos(m_xcenter, m_ycenter);
        }
        break;

        case WM_SETFOCUS:
            Win32App::m_focus = true;
        break;

        case WM_KILLFOCUS:
            Win32App::m_focus = false;
        break;

        case WM_DESTROY:
            PostQuitMessage(0);
        break;

        default:
            return DefWindowProc(hwnd, message, wParam, lParam);
    }

    return 0;
}