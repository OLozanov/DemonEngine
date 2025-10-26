#include "Win32App.h"
#include "Game\Game.h"

int WINAPI WinMain(HINSTANCE hThisInstance,
    HINSTANCE hPrevInstance,
    LPSTR lpszArgument,
    int nCmdShow)
{
    return Win32App::Run(hThisInstance, lpszArgument, nCmdShow);
}