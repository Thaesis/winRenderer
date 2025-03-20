#define UTILITY_H
#ifndef UTILTIY_H

#include <string>
#include <windows.h>

namespace util {

    
    std::string wstringToString(const std::wstring& str);
    std::wstring stringToWString(const std::string& str);
    std::wstring charToWString(const char* str);
    std::wstring getExecutablePath();
}

#endif