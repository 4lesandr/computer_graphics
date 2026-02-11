#pragma once
#pragma comment(lib, "d3d11.lib")
#pragma comment(lib, "dxgi.lib")
#pragma comment(lib, "d3dcompiler.lib")
#pragma comment(lib, "dxguid.lib")
#pragma comment(lib, "winmm.lib")

#include <windows.h>
#include <d3d11.h>
#include <dxgi.h>
#include <tchar.h>

#define SAFE_RELEASE(p) { if (p) { (p)->Release();  (p) = nullptr; } }

class ColorWindow
{
public:
    ColorWindow(HINSTANCE hInstance);
    ~ColorWindow();

    bool initialize();
    int run();

private:
    bool initWindow();
    static LRESULT CALLBACK windowProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);

    bool initDirectX();
    bool initSwapChain();
    void resizeSwapChain(UINT width, UINT height);
    void renderFrame();
    void cleanupDirectX();

    HINSTANCE m_hInstance;
    HWND m_hWnd;
    const LPCWSTR m_windowClassName = L"DirectXWindowClass";
    const LPCWSTR m_windowTitle = L"DirectX 11 Application";

    UINT m_windowWidth = 1280;
    UINT m_windowHeight = 720;

    IDXGIFactory* m_pFactory = nullptr;
    IDXGIAdapter* m_pAdapter = nullptr;
    ID3D11Device* m_pDevice = nullptr;
    ID3D11DeviceContext* m_pDeviceContext = nullptr;
    IDXGISwapChain* m_pSwapChain = nullptr;
    ID3D11RenderTargetView* m_pBackBufferRTV = nullptr;

    bool m_exitRequested = false;
};
