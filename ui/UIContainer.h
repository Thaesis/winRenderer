#ifndef UICONTAINER_H
#define UICONTAINER_H

#include <string>
#include <vector>
#include <memory>
#include <math.h>
#include <windows.h>
#include "../util/Metrics.h"

enum UIContainerType {ROOT, INVENTORY, MAP, STATS, LOG};

struct localRect {
    float left;
    float top;
    float right;
    float bottom;

    localRect(float left, float top, float right, float bottom) : left(left), top(top), right(right), bottom(bottom) {}
    RECT toPixel(HWND hwnd) const {

        RECT clientRect;
        GetClientRect(hwnd, &clientRect);
        int clientWidth = clientRect.right - clientRect.left;
        int clientHeight = clientRect.bottom - clientRect.top;

        return {
            static_cast<LONG>(((left + 1.0f) * 0.5f) * clientWidth),
            static_cast<LONG>(((top + 1.0f) * 0.5f) * clientHeight),
            static_cast<LONG>(((right + 1.0f) * 0.5f) * clientWidth),
            static_cast<LONG>(((bottom + 1.0f) * 0.5f) * clientHeight)
        };
    }
};
 
class UIContainer {
    private:
        // Attributes
        std::wstring label;
        std::wstring content;
        localRect rect;
        UIContainerType type;
        UIContainer* parent;
        std::vector<std::unique_ptr<UIContainer>> children;
        // active content provider? 

    public:

        explicit UIContainer(std::string label, UIContainerType type, UIContainer* parent = nullptr, float x = 0, float y = 0, float width = 0, float height = 0);
        
        const std::vector<std::unique_ptr<UIContainer>>& getChildren() const;
        const localRect& getRect() const;
        UIContainer* getParent() const;
        std::wstring getLabel() const;
        void addChildContainer(std::unique_ptr<UIContainer> child);

        // Accessors
        RECT getActiveContainerRegion(); //TODO

};

#endif