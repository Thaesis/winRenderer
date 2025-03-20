#ifndef GAMEWINDOW_H
#define GAMEWINDOW_H

#define TEXT_FILE_LOC "\\assets\\font\\tfont.fnt"

#include <windows.h>
#include "TextRenderer.h"
#include "../ui/UIContainer.h"
#include <string>
#include <vector>
#include <memory>
#include <codecvt>
#include <locale>
#include <iostream>

class GameWindow {
    private:

        // Attributes
        HWND hwnd;
        RECT windowedRect;
        HBITMAP hBitmap;
        TextRenderer tRenderer;
        bool fullscreen = false;
        bool isInitalized = false;

        std::unique_ptr<UIContainer> rootContainer;

        //Function Declarations
        void updateWindow(HDC hdc, HDC hMemDC, UIContainer* root);
        void drawContainerBorder(HDC hdc, HDC hMemDC, const localRect& rect, std::wstring label);
        void loadBitmap();

    public:

        const float ASPECT_RATIO = 16.0f / 9.0f;

        //Function Declarations
        GameWindow();
        ~GameWindow();
        HWND create(HINSTANCE hInstance, int width, int height, const char* title);
        void setRootContainer(std::unique_ptr<UIContainer> root);
        UIContainer* getRootContainer() const;
        void show(int nCmdShow);
        void runMessageLoop();
        void toggleFullscreen();
        void forceRedraw();
        bool initialized();

        static LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
};

#endif //GAMEWINDOW_H