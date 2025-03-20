
#include "UIContainer.h"
#include "../util/Utility.h"

UIContainer::UIContainer(std::string label, UIContainerType type, UIContainer* parent, float x, float y, float width, float height)
                     : label(util::stringToWString(label)), type(type), parent(parent), rect(localRect(x, y, x + width, y + height))
{
}

const localRect& UIContainer::getRect() const {
    return rect;
}

const std::vector<std::unique_ptr<UIContainer>>& UIContainer::getChildren() const {
    return children;
}

std::wstring UIContainer::getLabel() const {
    return label;
}

UIContainer* UIContainer::getParent() const {
    return parent;
}

void UIContainer::addChildContainer(std::unique_ptr<UIContainer> child) {
    children.push_back(std::move(child));
}