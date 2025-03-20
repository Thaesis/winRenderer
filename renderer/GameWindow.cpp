#define UNICODE
#define _UNICODE

#include "GameWindow.h"
#include "../util/Utility.h"
#include "../ui/UIContainer.h"
#include <iostream>
#include <stdexcept>

GameWindow::GameWindow() : hwnd(nullptr), tRenderer(TextRenderer(TEXT_FILE_LOC)) {
}

GameWindow::~GameWindow() {
    if (hwnd)
        DestroyWindow(hwnd);

    if (hBitmap) {
        DeleteObject(hBitmap);
        hBitmap = nullptr;
    }
}

HWND GameWindow::create(HINSTANCE hInstance, int width, int height, const char* title) {
    WNDCLASS wc = {};
    wc.lpfnWndProc = WindowProc;
    wc.hInstance = hInstance;
    wc.lpszClassName = L"GameWindowClass";
    wc.hbrBackground = (HBRUSH)CreateSolidBrush(RGB(0,0,0));
    wc.hCursor = LoadCursor(NULL, IDC_ARROW);

    RegisterClass(&wc);

    // Genereate the wide-string to pass as arugment to CreateWindowEx()
    std::wstring wTitle = util::charToWString(title);

    hwnd = CreateWindowExW(
        0,
        L"GameWindowClass",
        wTitle.c_str(),
        WS_OVERLAPPEDWINDOW,
        CW_USEDEFAULT,
        CW_USEDEFAULT,
        width,
        height,
        NULL,
        NULL,
        hInstance,
        this
    );

    if (!hwnd) {
        std::cerr << "Error: Failed to create GameWindow -> hwnd == NULL" << std::endl;
    }

    SetWindowLongPtr(hwnd, GWLP_USERDATA, (LONG_PTR)this);

    return hwnd;
}

void GameWindow::show(int nCmdShow) {
    if (hwnd) {
        ShowWindow(hwnd, nCmdShow);
        UpdateWindow(hwnd);
    }
}

void GameWindow::runMessageLoop() {
    MSG msg = {};
    
    while (true) {

        while(PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE)) {

            if(msg.message == WM_QUIT) return;

            TranslateMessage(&msg);
            DispatchMessage(&msg);

        }

        forceRedraw();

    }

}

void GameWindow::toggleFullscreen() {

    GameWindow* self = (GameWindow*)GetWindowLongPtr(hwnd, GWLP_USERDATA);

    DWORD style = GetWindowLong(hwnd, GWL_STYLE);

    if (self->fullscreen) {

        SetWindowLong(hwnd, GWL_STYLE, style | WS_OVERLAPPEDWINDOW);
        SetWindowPos(hwnd, NULL, self->windowedRect.left, self->windowedRect.top,
             self->windowedRect.right - self->windowedRect.left, self->windowedRect.bottom - self->windowedRect.top,
            SWP_FRAMECHANGED | SWP_NOZORDER | SWP_NOOWNERZORDER);

    } else {

        GetWindowRect(hwnd, &windowedRect);

        SetWindowLong(hwnd, GWL_STYLE, style & ~WS_OVERLAPPEDWINDOW);

        MONITORINFO mi = {sizeof(mi)};

        if (GetMonitorInfo(MonitorFromWindow(hwnd, MONITOR_DEFAULTTOPRIMARY), &mi)) {
            SetWindowPos(hwnd, NULL, mi.rcMonitor.left, mi.rcMonitor.top, mi.rcMonitor.right - mi.rcMonitor.left,
            mi.rcMonitor.bottom - mi.rcMonitor.top,
            SWP_FRAMECHANGED | SWP_NOZORDER | SWP_NOOWNERZORDER);
        }

    }

    self->fullscreen = !self->fullscreen;

}

void GameWindow::drawContainerBorder(HDC hdc, HDC hMemDC, const localRect& rect, std::wstring label) {

    RECT r = rect.toPixel(hwnd);

    std::printf("Drawing Container At: N(%f, %f, %f, %f)\n", rect.left, rect.top, rect.right, rect.bottom);
    std::printf("Drawing Container At: P(%d, %d, %d, %d)\n", r.left, r.top, r.right, r.bottom);

    int containerWidth = r.right - r.left;
    int dashSize = 6; // (5px + 1px padding)Increasing this value decreases the # of dashes
    int numDashes = containerWidth / (dashSize * PIXEL_SCALE_FACTOR);

    std::wstring horizontal = L"+";

    for (int i = 0; i < numDashes - 2; i++) {
        horizontal += L"-";
    } 
    horizontal += L'+';
    std::wstring vertical = L"|";

    tRenderer.drawString(hdc, hMemDC, horizontal.c_str(), r.left, r.top);

    for (int y = r.top + 15; y < r.bottom - 15; y += 15) {
        tRenderer.drawString(hdc, hMemDC, vertical.c_str(), r.left, y);
        tRenderer.drawString(hdc, hMemDC, vertical.c_str(), r.right - 6, y);
    }

    tRenderer.drawString(hdc, hMemDC, horizontal.c_str(), r.left, r.bottom - 15);

    tRenderer.drawString(hdc, hMemDC, label.c_str(), r.left + 7, r.top + 10);

    GdiFlush();

}

void GameWindow::updateWindow(HDC hdc, HDC hMemDC, UIContainer* root) {
    
    if (!root) {return;}

    // Prevents Drawing an outline at the root of the container tree.
    if (root->getParent() != nullptr) {
        drawContainerBorder(hdc, hMemDC, root->getRect(), root->getLabel());
    }

    for (const auto& child : root->getChildren()) {
        if (child) {
            updateWindow(hdc, hMemDC, child.get());
        }
    }
}

void GameWindow::forceRedraw() {
    if (hwnd) {
        InvalidateRect(hwnd, NULL, TRUE);
        UpdateWindow(hwnd);
    }
}

void GameWindow::setRootContainer(std::unique_ptr<UIContainer> root) {
    rootContainer = std::move(root);
    isInitalized = true;
    InvalidateRect(hwnd, NULL, TRUE);
}

UIContainer* GameWindow::getRootContainer() const {
    return rootContainer.get();
}
 
void GameWindow::loadBitmap() {
    
    if (hBitmap) {
        DeleteObject(hBitmap);
        hBitmap = nullptr;
    }

    hBitmap = tRenderer.getBitmap();

    if(!hBitmap) {
        std::cerr << "Error: Unable to retrieve Bitmap" << std::endl;
    } else {
        BITMAP bmp;
        GetObject(hBitmap, sizeof(BITMAP), &bmp);
        std::cout << "Bitmap Loaded" << bmp.bmWidth << "x" << bmp.bmHeight << std::endl;
    }

}

bool GameWindow::initialized() {
    return isInitalized;
}

LRESULT CALLBACK GameWindow::WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
    GameWindow* self = (GameWindow*)GetWindowLongPtr(hwnd, GWLP_USERDATA);

    if (!self) {
        
        if(uMsg == WM_CREATE) {
            std::cout << "WM_CREATE Caught, self is null (Setting it now)" << std::endl;
        } else {
            std::cerr << "Error: GameWindow instance is null! :: " << uMsg << std::endl;
            return DefWindowProc(hwnd, uMsg, wParam, lParam);
        }
    }

    switch (uMsg) {
        case WM_KEYDOWN: {

            // Keypress
            if (wParam == VK_F11)
                self->toggleFullscreen();

            break;
        }
        case WM_SIZING: {
            RECT* rect = (RECT*)lParam;
            int width = rect->right - rect->left;
            int height = rect->bottom - rect->top;

            if (width / (float)height > self->ASPECT_RATIO) {
                width = (int)(height * self->ASPECT_RATIO);
            } else {
                height = (int)(width / self->ASPECT_RATIO);
            }

            rect->right = rect->left + width;
            rect->bottom = rect->top + height;

            return TRUE;
        }
        case WM_SIZE: {

            int width = LOWORD(lParam);
            int height = HIWORD(lParam);
            break;

        }
        case WM_CREATE: {
            
            LPCREATESTRUCT pCreate = (LPCREATESTRUCT)lParam;
            self = (GameWindow*)pCreate->lpCreateParams;

            SetWindowLongPtr(hwnd, GWLP_USERDATA, (LONG_PTR)self);
            self->hwnd = hwnd;

            // Load bmp for the custom texture font
            self->loadBitmap();

            if(!self->hBitmap) {
                std::cerr << "Fatal: Failed to load BMP file." << std::endl;
            }

            break;

        }
        case WM_PAINT: {

            if(!self->initialized()) {
                break;
            }

            PAINTSTRUCT ps;

            HDC hdc = BeginPaint(hwnd, &ps);

            // Back Buffer for Double Buffer
            HDC hdcBuffer = CreateCompatibleDC(hdc);
            HBITMAP hbmBuffer = CreateCompatibleBitmap(hdc, ps.rcPaint.right, ps.rcPaint.bottom);
            HBITMAP hbmOldBuffer = (HBITMAP)SelectObject(hdcBuffer, hbmBuffer);

            // Mem DC for bitmap
            HDC hMemDC = CreateCompatibleDC(hdc);
            SelectObject(hMemDC, self->hBitmap);

            BITMAP bitmap;
            GetObject(self->hBitmap, sizeof(BITMAP), &bitmap);
            BitBlt(hdcBuffer, 100, 100, bitmap.bmWidth, bitmap.bmHeight, hMemDC, 0, 0, SRCCOPY);

            if (self && self->rootContainer) {
                self->updateWindow(hdcBuffer, hMemDC, self->getRootContainer());
            } else {
                std::cerr << "Error: rootContainer is null in WM_PAINT." << std::endl;
            }

            BitBlt(hdc, 0, 0, ps.rcPaint.right, ps.rcPaint.bottom, hdcBuffer, 0, 0, SRCCOPY);

            // Cleanup
            SelectObject(hMemDC, self->hBitmap);
            DeleteDC(hMemDC);

            SelectObject(hdcBuffer, hbmOldBuffer);
            DeleteObject(hbmBuffer);
            DeleteDC(hdcBuffer);
            
            EndPaint(hwnd, &ps);
            break; 
        } 
        case WM_DESTROY: {

            if (self->hBitmap) {
                DeleteObject(self->hBitmap);
            }

            PostQuitMessage(0);
            break;
        }
        case WM_CLOSE: {
            DestroyWindow(hwnd);
            break;
        }
        case WM_ERASEBKGND: {
            return 1;
        }
        default:
            return DefWindowProc(hwnd, uMsg, wParam, lParam);
    }

     return 0;

}
