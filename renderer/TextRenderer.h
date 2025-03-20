#ifndef TEXTRENDERER_H
#define TEXTRENDERER_H

#include <windows.h>
#include <wingdi.h>
#include <unordered_map>
#include <fstream>
#include <sstream>
#include <string>
#include <filesystem>
#include <iostream>
#include "../util/Metrics.h"
#include "../util/Utility.h"


struct FontChar {
    int id;
    int x, y;
    int width, height;
    int xoffset, yoffset;
    int xadvance;
};

class TextRenderer {
private:

    std::unordered_map<int, FontChar> fontChars;
    std::string fntFileName;
    std::wstring bmpFileName; 

public:

    TextRenderer();
    TextRenderer(const std::string& textureFileName);
    ~TextRenderer();
    bool loadFontFile(const std::string& fontFile);
    HBITMAP getBitmap();
    void drawChar(HDC hdc, HDC hMemDC, int charID, int x, int y);
    void drawString(HDC hdc, HDC hMemDC, const std::wstring& text, int startX, int startY);
};


#endif