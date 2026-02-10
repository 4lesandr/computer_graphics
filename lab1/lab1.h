#pragma once

#include <windows.h>
#include <d3d11.h>
#include <dxgi.h>
#include <assert.h>
#include <tchar.h>

// ћакросы дл€ безопасного освобождени€ ресурсов
#define SAFE_RELEASE(p) { if (p) { (p)->Release();  (p) = nullptr; } }

class ColorWindow
{
public:
    ColorWindow(HINSTANCE hInstance);
    ~ColorWindow();

    bool Initialize();
    int Run();

private:
    bool InitWindow();
    static LRESULT CALLBACK WindowProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);

    bool InitDirectX();
    bool InitSwapChain();
    void ResizeSwapChain(UINT width, UINT height);
    void RenderFrame();
    void CleanupDirectX();

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
