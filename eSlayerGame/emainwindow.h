#ifndef EMAINWINDOW_H
#define EMAINWINDOW_H

#include "widgets/ewidget.h"
#include "eresolution.h"
#include "ewindowsettings.h"

using eSlot = std::function<void()>;

class eMainWindow {
public:
    eMainWindow();
    ~eMainWindow();

    bool initialize(const eWindowSettings& settings);
public:
    void setWidget(eWidget* const w);
    eWidget* takeWidget();

    int exec();
    void quit();

    void addSlot(const eSlot& slot);
    void addLoad(const eAction& l);

    int width() const { return resolution().width(); }
    int height() const { return resolution().height(); }
    const eResolution& resolution() const { return mSettings.fRes; }
    SDL_Window* window() const { return mSdlWindow; }
    SDL_Renderer* renderer() const { return mSdlRenderer; }

    void setResolution(const eResolution& res);
    void setFullscreen(const bool f);

    eWidget* currentWidget() const { return mWidget; }

    const eWindowSettings& settings() const { return mSettings; }
private:
    eWindowSettings mSettings;

    bool mQuit = false;
    bool mFirstFullscrenSetting = true;
    bool mFirstResolutionSetting = true;

    std::vector<eSlot> mSlots;
    std::vector<eAction> mLoading;

    int mShiftPressed = 0;
    int mCtrlPressed = 0;

    eWidget* mWidget = nullptr;
    SDL_Window* mSdlWindow = nullptr;
    SDL_Renderer* mSdlRenderer = nullptr;
};

#endif // EMAINWINDOW_H
