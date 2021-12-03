#include <cstdint>
#include <Windows.h>

#include "../Direct3D/Direct3D.h"
#include "../Util/Assert.h"

wchar_t gAppName[] = L"DirectX11Sample";

LRESULT CALLBACK WndProc (
    _In_ HWND   hWnd,
    _In_ UINT   uMsg,
    _In_ WPARAM wParam,
    _In_ LPARAM lParam
);

bool InitWindow(HINSTANCE hInstance, const uint32_t width, const uint32_t height, HWND* hWnd);

int WINAPI wWinMain(
    _In_ HINSTANCE hInstance,
    _In_opt_ HINSTANCE hPrevInstance,
    _In_ LPWSTR lpCmdLine,
    _In_ int nShowCmd
)
{
    constexpr uint32_t WIDTH = 800;
    constexpr uint32_t HEIGHT = 600;
    HWND hWnd;
    MSG msg;

    if (!InitWindow(hInstance, WIDTH, HEIGHT, &hWnd))
    {
        AssertW(false, L"failed to create window");
        return -1;
    }

    Direct3D* d3d = new Direct3D;
    if (!d3d->Initialize(hWnd, hInstance))
    {
        goto EXIT;
    }

    while (true)
    {
        if (PeekMessageW(&msg, nullptr, 0, 0, PM_REMOVE))
        {
            if (msg.message == WM_QUIT)
            {
                break;
            }

            TranslateMessage(&msg);
            DispatchMessageW(&msg);
        }
        else
        {
            // game loop
            d3d->Render();
        }
    }

EXIT:
    delete d3d;
	return 0;
}

LRESULT CALLBACK WndProc(
    _In_ HWND   hWnd,
    _In_ UINT   uMsg,
    _In_ WPARAM wParam,
    _In_ LPARAM lParam
)
{
    switch (uMsg)
    {
    case WM_DESTROY:
        PostQuitMessage(0);
        break;
    case WM_PAINT:
    {
        PAINTSTRUCT ps;
        HDC hdc = BeginPaint(hWnd, &ps);
        EndPaint(hWnd, &ps);
        break;
    }
    default:
        return DefWindowProcW(hWnd, uMsg, wParam, lParam);
    }

    return 0;
}

bool InitWindow(HINSTANCE hInstance, const uint32_t width, const uint32_t height, HWND* hWnd)
{
    WNDCLASSEXW wc = {};
    wc.cbSize = sizeof(WNDCLASSEXW);
    wc.style = CS_HREDRAW | CS_VREDRAW;
    wc.lpfnWndProc = WndProc;
    wc.hInstance = hInstance;
    wc.hIcon = nullptr;
    wc.hCursor = nullptr;
    wc.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
    wc.lpszMenuName = nullptr;
    wc.lpszClassName = gAppName;
    wc.hIconSm = nullptr;

    RegisterClassExW(&wc);

    RECT windowRect = { 0, 0, (LONG)width, (LONG)height };
    AdjustWindowRect(&windowRect, WS_OVERLAPPEDWINDOW, false);

    *hWnd = CreateWindowExW(0, gAppName, gAppName, WS_OVERLAPPED | WS_CAPTION | WS_MINIMIZEBOX | WS_SYSMENU,
        CW_USEDEFAULT, CW_USEDEFAULT, windowRect.right - windowRect.left, windowRect.bottom - windowRect.top,
        nullptr, nullptr, hInstance, nullptr);
    if (!(*hWnd))
    {
        return false;
    }

    ShowWindow(*hWnd, SW_SHOWDEFAULT);

    return true;
}