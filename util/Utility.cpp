#include "Utility.h"

std::string util::wstringToString(const std::wstring& wstr) {
    size_t len = std::wcstombs(nullptr, wstr.c_str(), 0);

    if (len == static_cast<size_t>(-1)) return "";
    std::string str(len, '\0');
    std::wcstombs(&str[0], wstr.c_str(), len);
    return str;
}

std::wstring util::stringToWString(const std::string& str) {
    return std::wstring(str.begin(), str.end());
}

std::wstring util::charToWString(const char* str) {
    int size = MultiByteToWideChar(CP_UTF8, 0, str, -1, NULL, 0);
    std::wstring wstr(size, 0);
    MultiByteToWideChar(CP_UTF8, 0, str, -1, &wstr[0], size);
    return wstr;
}

std::wstring util::getExecutablePath() {
    wchar_t buffer[MAX_PATH];
    GetModuleFileNameW(NULL, buffer, MAX_PATH);

    std::wstring exePath(buffer);

    size_t pos = exePath.find_last_of(L"\\/");
    return (pos != std::wstring::npos) ? exePath.substr(0, pos) : exePath;
}