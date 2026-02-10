#include <dxgi1_2.h>
#include "lab1.h"

static const FLOAT clearColor[4] = { 0.1f, 0.2f, 0.6f, 1.0f };

ColorWindow::ColorWindow(HINSTANCE hInstance)
    : m_hInstance(hInstance)
{
}

ColorWindow::~ColorWindow()
{
    CleanupDirectX();
}

bool ColorWindow::Initialize()
{
    if (!InitWindow())
        return false;

    if (!InitDirectX())
        return false;

    ShowWindow(m_hWnd, SW_SHOW);
    UpdateWindow(m_hWnd);

    return true;
}

int ColorWindow::Run()
{
    MSG msg = { 0 };

    while (!m_exitRequested)
    {
        if (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE))
        {
            if (msg.message == WM_QUIT)
                m_exitRequested = true;

            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
        else
        {
            RenderFrame();
        }
    }

    return (int)msg.wParam;
}

bool ColorWindow::InitWindow()
{
    WNDCLASSEX wcex;
    wcex.cbSize = sizeof(WNDCLASSEX);
    wcex.style = CS_HREDRAW | CS_VREDRAW;
    wcex.lpfnWndProc = WindowProc;
    wcex.cbClsExtra = 0;
    wcex.cbWndExtra = 0;
    wcex.hInstance = m_hInstance;
    wcex.hIcon = LoadIcon(nullptr, IDI_APPLICATION);
    wcex.hCursor = LoadCursor(nullptr, IDC_ARROW);
    wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
    wcex.lpszMenuName = nullptr;
    wcex.lpszClassName = m_windowClassName;
    wcex.hIconSm = LoadIcon(nullptr, IDI_APPLICATION);

    if (!RegisterClassEx(&wcex))
    {
        MessageBox(nullptr, L"Ошибка регистрации класса окна!", L"Ошибка", MB_OK);
        return false;
    }

    RECT rc = { 0, 0, (LONG)m_windowWidth, (LONG)m_windowHeight };
    AdjustWindowRect(&rc, WS_OVERLAPPEDWINDOW, FALSE);

    m_hWnd = CreateWindow(
        m_windowClassName,
        m_windowTitle,
        WS_OVERLAPPEDWINDOW,
        CW_USEDEFAULT, CW_USEDEFAULT,
        rc.right - rc.left,
        rc.bottom - rc.top,
        nullptr,
        nullptr,
        m_hInstance,
        this
    );

    if (!m_hWnd)
    {
        MessageBox(nullptr, L"ошибка создания окна", L"Ошибка", MB_OK);
        return false;
    }

    return true;
}

bool ColorWindow::InitDirectX()
{
    HRESULT hr = S_OK;

    hr = CreateDXGIFactory(__uuidof(IDXGIFactory), (void**)&m_pFactory);
    if (FAILED(hr))
    {
        MessageBox(nullptr, L"Ошибка создания DXGI Factory", L"Ошибка", MB_OK);
        return false;
    }

    UINT adapterIndex = 0;
    bool adapterFound = false;

    while (SUCCEEDED(m_pFactory->EnumAdapters(adapterIndex, &m_pAdapter)))
    {
        DXGI_ADAPTER_DESC desc;
        m_pAdapter->GetDesc(&desc);

        if (wcscmp(desc.Description, L"Microsoft Basic Render Driver") != 0)
        {
            adapterFound = true;
            break;
        }

        m_pAdapter->Release();
        m_pAdapter = nullptr;
        adapterIndex++;
    }

    if (!adapterFound)
    {
        MessageBox(nullptr, L"Не найден подходящий графический адаптер!", L"Ошибка", MB_OK);
        return false;
    }

    D3D_FEATURE_LEVEL featureLevel;
    D3D_FEATURE_LEVEL featureLevels[] = { D3D_FEATURE_LEVEL_11_0 };

    UINT createDeviceFlags = 0;
#ifdef _DEBUG
    createDeviceFlags |= D3D11_CREATE_DEVICE_DEBUG;
#endif

    hr = D3D11CreateDevice(
        m_pAdapter,
        D3D_DRIVER_TYPE_UNKNOWN,
        nullptr,
        createDeviceFlags,
        featureLevels,
        _countof(featureLevels),
        D3D11_SDK_VERSION,
        &m_pDevice,
        &featureLevel,
        &m_pDeviceContext
    );

    if (FAILED(hr))
    {
        MessageBox(nullptr, L"Ошибка создания устройства DirectX 11!", L"Ошибка", MB_OK);
        return false;
    }

    if (!InitSwapChain())
        return false;

    return true;
}

bool ColorWindow::InitSwapChain()
{
    HRESULT hr = S_OK;

    IDXGIDevice* pDXGIDevice = nullptr;
    hr = m_pDevice->QueryInterface(__uuidof(IDXGIDevice), (void**)&pDXGIDevice);

    IDXGIAdapter* pDXGIAdapter = nullptr;
    hr = pDXGIDevice->GetAdapter(&pDXGIAdapter);

    IDXGIFactory* pFactory = nullptr;
    hr = pDXGIAdapter->GetParent(__uuidof(IDXGIFactory), (void**)&pFactory);

    DXGI_SWAP_CHAIN_DESC swapChainDesc = { 0 };
    swapChainDesc.BufferCount = 2;
    swapChainDesc.BufferDesc.Width = m_windowWidth;
    swapChainDesc.BufferDesc.Height = m_windowHeight;
    swapChainDesc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    swapChainDesc.BufferDesc.RefreshRate.Numerator = 0;
    swapChainDesc.BufferDesc.RefreshRate.Denominator = 1;
    swapChainDesc.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
    swapChainDesc.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;
    swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
    swapChainDesc.OutputWindow = m_hWnd;
    swapChainDesc.SampleDesc.Count = 1;
    swapChainDesc.SampleDesc.Quality = 0;
    swapChainDesc.Windowed = TRUE;
    swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
    swapChainDesc.Flags = 0;

    hr = pFactory->CreateSwapChain(m_pDevice, &swapChainDesc, &m_pSwapChain);

    SAFE_RELEASE(pFactory);
    SAFE_RELEASE(pDXGIAdapter);
    SAFE_RELEASE(pDXGIDevice);

    if (FAILED(hr))
    {
        MessageBox(nullptr, L"Ошибка создания Swap Chain!", L"Ошибка", MB_OK);
        return false;
    }

    ResizeSwapChain(m_windowWidth, m_windowHeight);

    return true;
}

void ColorWindow::ResizeSwapChain(UINT width, UINT height)
{
    if (!m_pSwapChain || width == 0 || height == 0)
        return;

    SAFE_RELEASE(m_pBackBufferRTV);

    HRESULT hr = m_pSwapChain->ResizeBuffers(
        2,
        width,
        height,
        DXGI_FORMAT_R8G8B8A8_UNORM,
        0
    );

    if (FAILED(hr))
    {
        MessageBox(nullptr, L"Ошибка изменения размера Swap Chain!", L"Ошибка", MB_OK);
        return;
    }

    ID3D11Texture2D* pBackBuffer = nullptr;
    hr = m_pSwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (LPVOID*)&pBackBuffer);

    if (SUCCEEDED(hr))
    {
        hr = m_pDevice->CreateRenderTargetView(pBackBuffer, nullptr, &m_pBackBufferRTV);
        SAFE_RELEASE(pBackBuffer);
    }

    if (FAILED(hr))
    {
        MessageBox(nullptr, L"Ошибка создания Render Target View!", L"Ошибка", MB_OK);
    }

    m_windowWidth = width;
    m_windowHeight = height;
}

void ColorWindow::RenderFrame()
{
    if (!m_pDeviceContext || !m_pBackBufferRTV)
        return;

    m_pDeviceContext->ClearRenderTargetView(m_pBackBufferRTV, clearColor);

    HRESULT hr = m_pSwapChain->Present(1, 0);
    if (FAILED(hr))
    {
        if (hr == DXGI_ERROR_DEVICE_REMOVED || hr == DXGI_ERROR_DEVICE_RESET)
        {
            CleanupDirectX();
            InitDirectX();
        }
    }
}

void ColorWindow::CleanupDirectX()
{
    SAFE_RELEASE(m_pBackBufferRTV);
    SAFE_RELEASE(m_pSwapChain);
    SAFE_RELEASE(m_pDeviceContext);
    SAFE_RELEASE(m_pDevice);
    SAFE_RELEASE(m_pAdapter);
    SAFE_RELEASE(m_pFactory);
}

LRESULT CALLBACK ColorWindow::WindowProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    ColorWindow* pApp = nullptr;

    if (message == WM_CREATE)
    {
        CREATESTRUCT* pCreate = reinterpret_cast<CREATESTRUCT*>(lParam);
        pApp = reinterpret_cast<ColorWindow*>(pCreate->lpCreateParams);
        SetWindowLongPtr(hWnd, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(pApp));
    }
    else
    {
        pApp = reinterpret_cast<ColorWindow*>(GetWindowLongPtr(hWnd, GWLP_USERDATA));
    }

    if (pApp)
    {
        switch (message)
        {
        case WM_SIZE:
            if (wParam != SIZE_MINIMIZED)
            {
                UINT width = LOWORD(lParam);
                UINT height = HIWORD(lParam);
                pApp->ResizeSwapChain(width, height);
            }
            break;

        case WM_DESTROY:
            PostQuitMessage(0);
            break;

        case WM_KEYDOWN:
            if (wParam == VK_ESCAPE)
                PostQuitMessage(0);
            break;

        default:
            return DefWindowProc(hWnd, message, wParam, lParam);
        }
    }
    else
    {
        return DefWindowProc(hWnd, message, wParam, lParam);
    }

    return 0;
}