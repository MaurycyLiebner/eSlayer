#ifndef ESETTINGSMENU_H
#define ESETTINGSMENU_H

#include "../widgets/ewidget.h"

#include "../ewindowsettings.h"

inline bool operator==(const eWindowSettings& s0,
                       const eWindowSettings& s1) {
    if(s0.fRes != s1.fRes) return false;
    if(s0.fFullscreen != s1.fFullscreen) return false;
    return true;
}

class eSettingsMenu : public eWidget {
public:
    eSettingsMenu(const eWindowSettings& iniSettings,
                  eMainWindow* const window);

    using eApplyAction = std::function<void(const eWindowSettings&)>;
    using eFullscreenA = std::function<void(const bool)>;
    void initialize(const eApplyAction& settingsA,
                    const eFullscreenA& fullscreenA);

private:
    const eWindowSettings mIniSettings;
    eWindowSettings mSettings;
};

#endif // ESETTINGSMENU_H
