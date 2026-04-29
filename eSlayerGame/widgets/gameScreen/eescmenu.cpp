#include "eescmenu.h"

#include "eescmenubutton.h"
#include "../../elanguage.h"
#include "../../erendersettings.h"

void eESCMenu::initialize(const eAction& return_,
                          const eAction& exit) {
    removeChildren();

    const auto optionsB = new eESCMenuButton(
        eLanguage::text(5, 0), window());
    addWidget(optionsB);
    optionsB->setPressAction([this]() {
        showOptions();
    });

    const auto exitB = new eESCMenuButton(
        eLanguage::text(5, 1), window());
    addWidget(exitB);
    exitB->setPressAction([exit]() {
        if(exit) exit();
    });

    const auto returnB = new eESCMenuButton(
        eLanguage::text(5, 2), window());
    addWidget(returnB);
    returnB->setPressAction([return_]() {
        if(return_) return_();
    });

    const auto& res = resolution();
    const int p = res.hugePadding();
    stackVertically(p);
    fitContent();

    optionsB->align(eAlignment::hcenter);
    exitB->align(eAlignment::hcenter);
    returnB->align(eAlignment::hcenter);

    mReturn = return_;
    mExit = exit;

    align(eAlignment::center);
}

int eESCMenu::buttonMaxWidth() const {
    const auto& res = resolution();
    return res.centralWidgetLargeWidth();
}

void eESCMenu::showOptions() {
    removeChildren();

    const auto soundOptionsB = new eESCMenuButton(
        eLanguage::text(5, 6), window());
    addWidget(soundOptionsB);
    soundOptionsB->setPressAction([this]() {
        showSoundOptions();
    });

    const auto videoOptionsB = new eESCMenuButton(
        eLanguage::text(5, 7), window());
    addWidget(videoOptionsB);
    videoOptionsB->setPressAction([this]() {
        showVideoOptions();
    });

    const auto previousB = new eESCMenuButton(
        eLanguage::text(5, 5), window());
    addWidget(previousB);
    previousB->setPressAction([this]() {
        initialize(mReturn, mExit);
    });

    const auto& res = resolution();
    const int p = res.hugePadding();
    stackVertically(p);
    fitContent();

    soundOptionsB->align(eAlignment::hcenter);
    videoOptionsB->align(eAlignment::hcenter);
    previousB->align(eAlignment::hcenter);

    align(eAlignment::center);
}

void eESCMenu::showSoundOptions() {
    removeChildren();

    const auto soundOptionsB = new eESCMenuButton(
        eLanguage::text(5, 6), window());
    addWidget(soundOptionsB);

    const auto previousB = new eESCMenuButton(
        eLanguage::text(5, 5), window());
    addWidget(previousB);
    previousB->setPressAction([this]() {
        showOptions();
    });

    const auto& res = resolution();
    const int p = res.hugePadding();
    stackVertically(p);
    fitContent();

    soundOptionsB->align(eAlignment::hcenter);
    previousB->align(eAlignment::hcenter);

    align(eAlignment::center);
}

void eESCMenu::showVideoOptions() {
    removeChildren();

    const int w = buttonMaxWidth();

    const auto videoOptionsB = new eESCMenuButton(
        eLanguage::text(5, 7), window());
    addWidget(videoOptionsB);

    const auto onOffValues = {eLanguage::text(5, 10), // on
                              eLanguage::text(5, 11)}; // off

    const int onId = 0;
    const int offId = 1;

    const auto objectShadowA = [&](const int id) {
        eRenderSettings::sRenderObjectShadows = id == onId;
        eRenderSettings::write();
    };

    const auto objectShadowsB = new eESCMenuSwitchButton(
        eLanguage::text(5, 8),
        onOffValues,
        eRenderSettings::sRenderObjectShadows ? onId : offId,
        objectShadowA, window(), w);
    addWidget(objectShadowsB);

    const auto wallShadowA = [&](const int id) {
        eRenderSettings::sRenderWallShadows = id == onId;
        eRenderSettings::write();
    };

    const auto wallShadowsB = new eESCMenuSwitchButton(
        eLanguage::text(5, 9),
        onOffValues,
        eRenderSettings::sRenderWallShadows ? onId : offId,
        wallShadowA, window(), w);
    addWidget(wallShadowsB);

    const auto previousB = new eESCMenuButton(
        eLanguage::text(5, 5), window());
    addWidget(previousB);
    previousB->setPressAction([this]() {
        showOptions();
    });

    const auto& res = resolution();
    const int p = res.hugePadding();
    stackVertically(p);
    fitContent();

    videoOptionsB->align(eAlignment::hcenter);
    objectShadowsB->align(eAlignment::hcenter);
    wallShadowsB->align(eAlignment::hcenter);
    previousB->align(eAlignment::hcenter);

    align(eAlignment::center);
}
