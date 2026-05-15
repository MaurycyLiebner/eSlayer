#include "eescmenu.h"

#include "../../elanguage.h"
#include "../../erendersettings.h"
#include "eescmenubutton.h"

#include <eSlayerHelpers/evectorhelpers.h>

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

    const auto qualityStrs = {eLanguage::text(5, 11), // low
                              eLanguage::text(5, 10), // medium
                              eLanguage::text(5, 9)}; // high
    const std::vector<int> lightingQualityValues = {1, 3, 5};
    const int lqId = eVectorHelpers::index(lightingQualityValues,
                                           eRenderSettings::sLightingQuality);

    const auto lightingQualityA = [lightingQualityValues](const int id) {
        eRenderSettings::sLightingQuality = lightingQualityValues[id];
        eRenderSettings::write();
    };

    const auto lightingQualityB = new eESCMenuSwitchButton(
        eLanguage::text(5, 8), qualityStrs, lqId,
        lightingQualityA, window(), w);
    addWidget(lightingQualityB);

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
    lightingQualityB->align(eAlignment::hcenter);
    previousB->align(eAlignment::hcenter);

    align(eAlignment::center);
}
