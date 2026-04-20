#include "colorwindow.h"

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
    UNREFERENCED_PARAMETER(hPrevInstance);
    UNREFERENCED_PARAMETER(lpCmdLine);
    UNREFERENCED_PARAMETER(nCmdShow);

    ColorWindow app(hInstance);

    if (!app.initialize())
    {
        MessageBox(nullptr, L"Ошибка инициализации приложения", L"Ошибка", MB_OK);
        return -1;
    }

    return app.run();
}
