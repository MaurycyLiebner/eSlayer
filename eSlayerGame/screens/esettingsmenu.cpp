#include "esettingsmenu.h"

#include "../widgets/mainMenu/emainmenubutton.h"
#include "../widgets/echeckbutton.h"
#include "../names/elanguagenames.h"
#include "../etext.h"

#include <eSlayerHelpers/estringhelpers.h>

#include <queue>

eSettingsMenu::eSettingsMenu(const eWindowSettings& iniSettings,
                             eMainWindow* const window) :
    eScreenBase(window),
    mIniSettings(iniSettings),
    mSettings(iniSettings) {}

class eThreeColsMenu : public eScreenBase {
protected:
    using eScreenBase::eScreenBase;

    void initialize(const eAction& exitA,
                    const eAction& okA);
    bool addWidgetToCol(eWidget* const w);
private:
    int mY = 0;
    std::queue<eWidget*> mCols;
};

class eSubMenu : public eThreeColsMenu {
public:
    using eThreeColsMenu::eThreeColsMenu;
protected:
    void paintEvent(ePainter& p) override {
        const auto rect = eWidget::rect();
        p.fillRect(rect, SDL_Color{0, 0, 0, 255});
    }

    bool mousePressEvent(const eMouseEvent& e) override {
        return true;
    }

    bool mouseMoveEvent(const eMouseEvent& e) override {
        return true;
    }
};

class eResolutionMenu : public eSubMenu {
public:
    using eSubMenu::eSubMenu;

    void initialize(eWindowSettings& settings,
                    const eAction& updateButton) {
        mRes = settings.fRes;

        const auto exitA = [&]() {
            deleteLater();
        };
        const auto okA = [&, updateButton]() {
            settings.fRes = mRes;
            updateButton();
            deleteLater();
        };
        eThreeColsMenu::initialize(exitA, okA);

        const auto& ress = eResolution::sResolutions;

        const auto currentButton = std::make_shared<eCheckButton*>(nullptr);
        const int iMax = ress.size();
        for(int i = 0; i < iMax; i++) {
            const auto& r = ress[i];
            const auto b = new eCheckButton(window());
            b->setSmallPadding();
            b->setText(r.name());
            b->fitContent();
            if(r == mRes) {
                *currentButton = b;
                b->setChecked(true);
            }
            addWidgetToCol(b);
            b->setCheckAction([this, currentButton, b, r](const bool c) {
                if(!c) {
                    b->setChecked(true);
                    return;
                }
                if(*currentButton) {
                    (*currentButton)->setChecked(false);
                }
                *currentButton = b;
                mRes = eResolution(r.fWidth, r.fHeight);
            });
        }
    }
private:
    eResolution mRes{1280, 720};
};

class eLanguageMenu : public eSubMenu {
public:
    using eSubMenu::eSubMenu;

    void initialize(eWindowSettings& settings,
                    const eAction& updateButton) {
        mLanguage = settings.fLanguage;

        const auto exitA = [&]() {
            deleteLater();
        };
        const auto okA = [&, updateButton]() {
            settings.fLanguage = mLanguage;
            updateButton();
            deleteLater();
        };
        eThreeColsMenu::initialize(exitA, okA);

        const auto& ls = eLanguage::sLanguages;

        const auto currentButton = std::make_shared<eCheckButton*>(nullptr);
        const int iMax = ls.size();
        for(int i = 0; i < iMax; i++) {
            const auto& l = ls[i];
            const auto b = new eCheckButton(window());
            b->setSmallPadding();
            const auto& name = eLanguageNames::name(l.fName);
            b->setText(name);
            b->fitContent();
            if(l.fName == mLanguage.fName) {
                *currentButton = b;
                b->setChecked(true);
            }
            addWidgetToCol(b);
            b->setCheckAction([this, currentButton, b, l](const bool c) {
                if(!c) {
                    b->setChecked(true);
                    return;
                }
                if(*currentButton) {
                    (*currentButton)->setChecked(false);
                }
                *currentButton = b;
                mLanguage = l;
            });
        }
    }
private:
    eLanguage mLanguage = eLanguage::sLanguage;
};

class eThreadsMenu : public eSubMenu {
public:
    using eSubMenu::eSubMenu;

    void initialize(eWindowSettings& settings,
                    const eAction& updateButton) {
        mThreads = settings.fThreads;

        const auto exitA = [&]() {
            deleteLater();
        };
        const auto okA = [&, updateButton]() {
            settings.fThreads = mThreads;
            updateButton();
            deleteLater();
        };
        eThreeColsMenu::initialize(exitA, okA);

        std::vector<int> ts;
        for(int i = -1; i <= 16; i++) {
            ts.emplace_back(i);
        }

        const auto threadsName = [](const int t) {
            if(t < 0) return eText::text(4, 5);
            return std::to_string(t);
        };

        const auto currentButton = std::make_shared<eCheckButton*>(nullptr);
        const int iMax = ts.size();
        for(int i = 0; i < iMax; i++) {
            const auto t = ts[i];
            const auto b = new eCheckButton(window());
            b->setSmallPadding();
            const auto name = threadsName(t);
            b->setText(name);
            b->fitContent();
            if(t == mThreads) {
                *currentButton = b;
                b->setChecked(true);
            }
            addWidgetToCol(b);
            b->setCheckAction([this, currentButton, b, t](const bool c) {
                if(!c) {
                    b->setChecked(true);
                    return;
                }
                if(*currentButton) {
                    (*currentButton)->setChecked(false);
                }
                *currentButton = b;
                mThreads = t;
            });
        }
    }
private:
    int mThreads;
};

void eSettingsMenu::initialize(const eAction& exitA,
                               const eApplyAction& settingsA) {
    setExit(exitA);
    const auto okA = [this, settingsA]() {
        settingsA(mSettings);
    };

    const auto inner = eScreenBase::addInner();

    const auto buttonW = new eWidget(window());
    buttonW->setNoPadding();
    const int w = inner->width();
    const int h = inner->height();
    buttonW->resize(w, h);

    {
        const auto text = mSettings.fFullscreen ?
                              eText::text(4, 3) : // full screen
                              eText::text(4, 2); // windowed screen
        const auto fs = new eMainMenuButton(text, window());
        fs->fitContent();
        buttonW->addWidget(fs);
        fs->align(eAlignment::hcenter);

        fs->setPressAction([this, fs]() {
            const bool f = !mSettings.fFullscreen;
            mSettings.fFullscreen = f;
            fs->setText(f ? eText::text(4, 3) : // full screen
                            eText::text(4, 2)); // windowed screen
        });
    }
    {
        const auto text = mSettings.fRes.name();
        const auto res = new eMainMenuButton(text, window());
        res->fitContent();
        buttonW->addWidget(res);
        res->align(eAlignment::hcenter);

        res->setPressAction([this, res]() {
            const auto w = new eResolutionMenu(window());
            const int width = eWidget::width();
            const int height = eWidget::height();
            w->resize(width, height);
            const auto updataButton = [this, res]() {
                const auto text = mSettings.fRes.name();
                res->setText(text);
                res->fitContent();
                res->align(eAlignment::hcenter);
            };
            w->initialize(mSettings, updataButton);
            addWidget(w);
        });
    }
    {
        const auto name = mSettings.fLanguage.fName;
        const auto text = eLanguageNames::name(name);
        const auto ls = new eMainMenuButton(text, window());
        ls->fitContent();
        buttonW->addWidget(ls);
        ls->align(eAlignment::hcenter);

        ls->setPressAction([this, ls]() {
            const auto w = new eLanguageMenu(window());
            const int width = eWidget::width();
            const int height = eWidget::height();
            w->resize(width, height);
            const auto updataButton = [this, ls]() {
                const auto name = mSettings.fLanguage.fName;
                const auto text = eLanguageNames::name(name);
                ls->setText(text);
                ls->fitContent();
                ls->align(eAlignment::hcenter);
            };
            w->initialize(mSettings, updataButton);
            addWidget(w);
        });
    }
    {
        const auto getText = [](const int threads) {
            std::string threadsText;
            if(threads < 0) {
                threadsText = eText::text(4, 5);
            } else {
                threadsText = std::to_string(threads);
            }
            const auto textBase = eText::text(4, 4);
            const auto text = eStringHelpers::replaceAll(textBase, "%1", threadsText);
            return text;
        };
        const auto text = getText(mSettings.fThreads);
        const auto ls = new eMainMenuButton(text, window());
        ls->fitContent();
        buttonW->addWidget(ls);
        ls->align(eAlignment::hcenter);

        ls->setPressAction([this, ls, getText]() {
            const auto w = new eThreadsMenu(window());
            const int width = eWidget::width();
            const int height = eWidget::height();
            w->resize(width, height);
            const auto updataButton = [this, ls, getText]() {
                const auto text = getText(mSettings.fThreads);
                ls->setText(text);
                ls->fitContent();
                ls->align(eAlignment::hcenter);
            };
            w->initialize(mSettings, updataButton);
            addWidget(w);
        });
    }

    inner->addWidget(buttonW);
    buttonW->stackVertically();

    {
        const auto e = new eMainMenuButton(
            eText::text(4, 1), window());
        inner->addWidget(e);
        e->setPressAction(exitA);
        e->align(eAlignment::bottom | eAlignment::left);

        const auto o = new eMainMenuButton(
            eText::text(4, 0), window());
        inner->addWidget(o);
        o->setPressAction(okA);
        o->align(eAlignment::bottom | eAlignment::right);
    }
}

void eThreeColsMenu::initialize(const eAction& exitA,
                                const eAction& okA) {
    setExit(exitA);

    const auto inner = eScreenBase::addInner();

    const auto& res = resolution();
    const int p = res.largePadding();

    const int colm = 2*p;
    const int colw = (inner->width() - 2*colm)/3;
    const int colh = inner->height() - 4*p;

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

    mCols.push(col1);
    mCols.push(col2);
    mCols.push(col3);

    {
        const auto e = new eMainMenuButton(
            eText::text(4, 1), window());
        inner->addWidget(e);
        e->setPressAction(exitA);
        e->align(eAlignment::bottom | eAlignment::left);

        const auto o = new eMainMenuButton(
            eText::text(4, 0), window());
        inner->addWidget(o);
        o->setPressAction(okA);
        o->align(eAlignment::bottom | eAlignment::right);
    }
}

bool eThreeColsMenu::addWidgetToCol(eWidget* const w) {
    if(mCols.empty()) return false;
    const auto& res = resolution();
    const int p = res.largePadding();

    auto col = mCols.front();
    if(col->height() - mY - p < w->height()) {
        if(mCols.empty()) return false;
        mCols.pop();
        col = mCols.front();
        mY = 0;
    }

    col->addWidget(w);
    w->setWidth(col->width() - 2*p);
    w->setY(mY);
    w->align(eAlignment::hcenter);

    mY += w->height() + p;
    return true;
}
