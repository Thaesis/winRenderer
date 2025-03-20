

#include "TextRenderer.h"

TextRenderer::TextRenderer() : fntFileName(nullptr) {}

TextRenderer::TextRenderer(const std::string& fntFileName) : fntFileName(fntFileName) {
    loadFontFile(fntFileName);
}

TextRenderer::~TextRenderer() {}

bool TextRenderer::loadFontFile(const std::string& fontFile) {

    std::string execPath = util::wstringToString(util::getExecutablePath());
    std::string filePath = execPath + fontFile;

    if (!std::filesystem::exists(filePath)) {
        std::cerr << "Error: Defined Path " + filePath + " does not exist!" << std::endl;
    }
    
    std::ifstream file(filePath);

    if(!file) {
        std::cerr << "Error: File exists but could not be accessed." << std::endl;
    }

    std::string line;

    while (std::getline(file, line)) {
        std::stringstream ss(line);
        std::string type;
        ss >> type;

        if (type == "page") { 

            std::string token;

            while (ss >> token) {
                
                if (token.find("file=") != std::string::npos) {
                    std::string tmp = token.substr(6, token.size() - 7);
                    std::cout << tmp << std::endl;
                    bmpFileName = util::stringToWString(execPath) + L"\\assets\\font\\" + util::stringToWString(tmp);

                    if (!std::filesystem::exists(bmpFileName)) {
                        std::cerr << "Error: BMP file does not exist at " + util::wstringToString(bmpFileName) << std::endl;
                    }

                }

            }

        } else if (type == "char") {

            FontChar fchar = {};
            std::string token;

            while ( ss >> token) {

                if (token.find("id=") != std::string::npos) fchar.id = std::stoi(token.substr(3));
                else if (token.find("x=") != std::string::npos) fchar.x = std::stoi(token.substr(2));
                else if (token.find("y=") != std::string::npos) fchar.y = std::stoi(token.substr(2));
                else if (token.find("width=") != std::string::npos) fchar.width = std::stoi(token.substr(6));
                else if (token.find("height=") != std::string::npos) fchar.height = std::stoi(token.substr(7));
                else if (token.find("xoffset=") != std::string::npos) fchar.xoffset = std::stoi(token.substr(8));
                else if (token.find("yoffset=") != std::string::npos) fchar.yoffset = std::stoi(token.substr(8));
                else if (token.find("xadvance=") != std::string::npos) fchar.xadvance = std::stoi(token.substr(9));

            }
            fontChars[fchar.id] = fchar;
        }

    }
    file.close();
    return true;
}

void TextRenderer::drawChar(HDC hdc, HDC hMemDC, int charID, int x, int y) {

    auto itr = fontChars.find(charID);
    
    if (itr == fontChars.end()) return;

    const FontChar& fchar = itr->second;

    HBITMAP hBitmap = (HBITMAP)GetCurrentObject(hMemDC, OBJ_BITMAP);
    if(!hBitmap) {
        std::cerr << "hMemDC does not contain a valid bitmap" << std::endl;
    }

    COLORREF pixelColor = GetPixel(hMemDC, fchar.x, fchar.y);

    BOOL result = StretchBlt(
        hdc,
        x + fchar.xoffset, 
        y + fchar.yoffset, 
        fchar.width * PIXEL_SCALE_FACTOR, 
        fchar.height * PIXEL_SCALE_FACTOR, 
        hMemDC, 
        fchar.x, 
        fchar.y,
        fchar.width,
        fchar.height, 
        SRCCOPY
    );

    if (!result) {
        std::cerr << "BitBlt Failed!" << std::endl;
    }
}

void TextRenderer::drawString(HDC hdc, HDC hMemDC, const std::wstring& text, int startX, int startY) {

    int x = startX;

    for(wchar_t c : text) {
        drawChar(hdc, hMemDC, c, x * 2,  startY);
        x += fontChars[c].xadvance;
    }

}

HBITMAP TextRenderer::getBitmap() {
    return (HBITMAP)LoadImageW(NULL, bmpFileName.c_str(), IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE);
}