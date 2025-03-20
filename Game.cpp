#include "renderer/GameWindow.h"
#include "ui/UIContainer.h"
#include "util/Utility.h"
#include "util/Metrics.h"

#define running true

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow) {

    AllocConsole();
    freopen("CONOUT$", "w", stdout);
    freopen("CONOUT$", "w", stderr);
    freopen("CONIN$", "r", stdin);

    GameWindow gameWin;
    HWND hwnd = gameWin.create(hInstance, 800, 600, "Game");

    if (!hwnd) return 0;

    gameWin.show(nCmdShow);

    auto rootContainer = std::make_unique<UIContainer>("Root", ROOT, nullptr, -1.0f, -1.0f, 2.0f, 2.0f);
    gameWin.setRootContainer(std::move(rootContainer));

    UIContainer* root = gameWin.getRootContainer();

    auto mapContainer = std::make_unique<UIContainer>("Map", MAP, root, -1.0f, -1.0f, 1.5f, 1.25f);
    root->addChildContainer(std::move(mapContainer));

    auto logContainer = std::make_unique<UIContainer>("Log", LOG, root, -1.0f, 0.25f, 0.5f, 0.75f);
    root->addChildContainer(std::move(logContainer));
 
    auto inventoryContainer = std::make_unique<UIContainer>("Inventory", INVENTORY, root, 0.0f , 0.0f, 1.0f, 0.75f);
    root->addChildContainer(std::move(inventoryContainer));

    auto statsContainer = std::make_unique<UIContainer>("Stats", STATS, root, 0.5f, -1.0f, 0.5f, 1.25f);
    root->addChildContainer(std::move(statsContainer));

    gameWin.runMessageLoop();

    return 0;
} 