#include "esettingsmenu.h"

#include "../widgets/mainMenu/emainmenubutton.h"
#include "../widgets/echeckbutton.h"
#include "../elanguage.h"

eSettingsMenu::eSettingsMenu(const eWindowSettings& iniSettings,
                             eMainWindow* const window) :
    eScreenBase(window),
    mIniSettings(iniSettings),
    mSettings(iniSettings) {

}

void eSettingsMenu::initialize(const eAction& exitA,
                               const eApplyAction& settingsA,
                               const eFullscreenA& fullscreenA) {
    setExit(exitA);
    const auto inner = eScreenBase::addInner();

    const auto& res = resolution();
    const int p = res.largePadding();

    const int colm = 2*p;
    const int colw = (inner->width() - 2*colm)/3;
    const int colh = inner->height();

    const auto col1 = new eWidget(window());
    col1->setNoPadding();
    col1->setWidth(colw + 2*p);
    col1->setHeight(colh);
    inner->addWidget(col1);

    const auto col2 = new eWidget(window());
    col2->setNoPadding();
    col2->setWidth(colw - p);
    col2->setHeight(colh);
    inner->addWidget(col2);
    col2->setX(colw + 2*p + colm);

    const auto col3 = new eWidget(window());
    col3->setNoPadding();
    col3->setWidth(colw - p);
    col3->setHeight(colh);
    inner->addWidget(col3);
    col3->setX(2*colw + 2*p + 2*colm);

    {
        const auto& res = mSettings.fRes;
        const auto& ress = eResolution::sResolutions;
        int y = 0;
        eWidget* col = col2;
        const auto currentButton = std::make_shared<eCheckButton*>(nullptr);
        const int iMax = ress.size();
        for(int i = 0; i < iMax; i++) {
            const auto& r = ress[i];
            const auto b = new eCheckButton(window());
            b->setSmallPadding();
            b->setText(r.name());
            b->fitContent();
            b->setWidth(col->width() - 2*p);
            if(r == res) {
                *currentButton = b;
                b->setChecked(true);
            }
            col->addWidget(b);
            b->setY(y);
            b->align(eAlignment::hcenter);
            b->setCheckAction([this, currentButton, b, r](const bool c) {
                if(!c) {
                    b->setChecked(true);
                    return;
                }
                if(*currentButton) {
                    (*currentButton)->setChecked(false);
                }
                *currentButton = b;
                mSettings.fRes = r;
            });
            y += b->height() + p;
            if(col->height() - y < b->height()) {
                col = col3;
                y = 0;
            }
        }
    }

    {
        const auto text = mSettings.fFullscreen ?
                              eLanguage::text(4, 2) : // windowed screen
                              eLanguage::text(4, 3); // full screen
        const auto fs = new eMainMenuButton(text, window());
        fs->fitContent();
        col1->addWidget(fs);

        fs->setPressAction([this, fs, fullscreenA]() {
            const bool f = !mSettings.fFullscreen;
            mSettings.fFullscreen = f;
            fullscreenA(f);
            fs->setText(f ? eLanguage::text(4, 2) : // windowed screen
                            eLanguage::text(4, 3)); // full screen
            fs->fitContent();
            fs->align(eAlignment::hcenter);
        });
        fs->align(eAlignment::hcenter);
    }

    col1->layoutVertically();

    {
        const auto e = new eMainMenuButton(
            eLanguage::text(4, 1), window());
        inner->addWidget(e);
        e->setPressAction(exitA);
        e->align(eAlignment::bottom | eAlignment::left);

        const auto o = new eMainMenuButton(
            eLanguage::text(4, 0), window());
        inner->addWidget(o);
        o->setPressAction([this, settingsA]() {
            settingsA(mSettings);
        });
        o->align(eAlignment::bottom | eAlignment::right);
    }
}
