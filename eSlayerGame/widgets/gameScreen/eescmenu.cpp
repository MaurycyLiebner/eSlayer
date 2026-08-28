#include "eescmenu.h"

#include "../../etext.h"
#include "../../erendersettings.h"
#include "../../widgets/eslider.h"
#include "../../audio/emusicplayer.h"
#include "../../audio/esoundplayer.h"
#include "../../audio/esoundeffectplayer.h"
#include "../../esoundoptions.h"
#include "eescmenubutton.h"

#include <eSlayerHelpers/evectorhelpers.h>

void eESCMenu::initialize(const eAction& return_,
                          const eAction& exit) {
    removeChildren();

    const auto optionsB = new eESCMenuButton(
        eText::text(5, 0), window());
    addWidget(optionsB);
    optionsB->setPressAction([this]() {
        showOptions();
    });

    const auto exitB = new eESCMenuButton(
        eText::text(5, 1), window());
    addWidget(exitB);
    exitB->setPressAction([exit]() {
        if(exit) exit();
    });

    const auto returnB = new eESCMenuButton(
        eText::text(5, 2), window());
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
        eText::text(5, 6), window());
    addWidget(soundOptionsB);
    soundOptionsB->setPressAction([this]() {
        showSoundOptions();
    });

    const auto videoOptionsB = new eESCMenuButton(
        eText::text(5, 7), window());
    addWidget(videoOptionsB);
    videoOptionsB->setPressAction([this]() {
        showVideoOptions();
    });

    const auto previousB = new eESCMenuButton(
        eText::text(5, 5), window());
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

    const auto musicOptionsB = new eESCMenuButton(
        eText::text(5, 12), window());
    addWidget(musicOptionsB);
    const int h = musicOptionsB->height()/2;

    const auto musicSlider = new eSlider(window());
    addWidget(musicSlider);
    musicSlider->setValue(eSoundOptions::sMusicVolume);
    musicSlider->setHeight(h);
    musicSlider->setSetter([](const int value) {
        eMusicPlayer::setVolume(0.01f*value);
        eSoundOptions::sMusicVolume = value;
        eSoundOptions::write();
    });

    const auto soundOptionsB = new eESCMenuButton(
        eText::text(5, 13), window());
    addWidget(soundOptionsB);

    const auto soundSlider = new eSlider(window());
    addWidget(soundSlider);
    soundSlider->setValue(eSoundOptions::sSoundVolume);
    soundSlider->setHeight(h);
    soundSlider->setSetter([](const int value) {
        eSoundPlayer::setVolume(0.01f*value);
        eSoundOptions::sSoundVolume = value;
        eSoundOptions::write();
    });

    const auto effectsOptionsB = new eESCMenuButton(
        eText::text(5, 14), window());
    addWidget(effectsOptionsB);

    const auto effectsSlider = new eSlider(window());
    addWidget(effectsSlider);
    effectsSlider->setValue(eSoundOptions::sSoundVolume);
    effectsSlider->setHeight(h);
    effectsSlider->setSetter([](const int value) {
        eSoundEffectPlayer::setVolume(0.01f*value);
        eSoundOptions::sEffectsVolume = value;
        eSoundOptions::write();
    });

    const auto previousB = new eESCMenuButton(
        eText::text(5, 5), window());
    addWidget(previousB);
    previousB->setPressAction([this]() {
        showOptions();
    });

    const auto& res = resolution();
    const int p = res.hugePadding();
    stackVertically(p);
    fitContent();

    const int w = width();
    musicSlider->setWidth(w);
    soundSlider->setWidth(w);
    effectsSlider->setWidth(w);

    soundOptionsB->align(eAlignment::hcenter);
    musicOptionsB->align(eAlignment::hcenter);
    effectsOptionsB->align(eAlignment::hcenter);
    previousB->align(eAlignment::hcenter);

    align(eAlignment::center);
}

void eESCMenu::showVideoOptions() {
    removeChildren();

    const int w = buttonMaxWidth();

    const auto videoOptionsB = new eESCMenuButton(
        eText::text(5, 7), window());
    addWidget(videoOptionsB);

    const auto qualityStrs = {eText::text(5, 11), // low
                              eText::text(5, 10), // medium
                              eText::text(5, 9)}; // high
    const auto name = eRenderSettings::sLightingQuality.fName;
    int lqId;
    if(name == "low") {
        lqId = 0;
    } else if(name == "medium") {
        lqId = 1;
    } else if(name == "high") {
        lqId = 2;
    } else {
        lqId = 0;
    }

    const auto lightingQualityA = [](const int id) {
        eRenderSettings::sLightingQuality = eRenderSettings::sLightingQualityOptions[id];
        eRenderSettings::write();
    };

    const auto lightingQualityB = new eESCMenuSwitchButton(
        eText::text(5, 8), qualityStrs, lqId,
        lightingQualityA, window(), w);
    addWidget(lightingQualityB);

    const auto previousB = new eESCMenuButton(
        eText::text(5, 5), window());
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
