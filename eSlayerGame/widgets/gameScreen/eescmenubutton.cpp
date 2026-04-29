#include "eescmenubutton.h"

eESCMenuButton::eESCMenuButton(const std::string& text,
                               eMainWindow * const window) :
    eButtonBase(window) {
    setHugeFontSize();
    setFontColor(eFontColor::whiteBlack);
    setText(text);
    fitContent();
}

void eESCMenuButton::paintEvent(ePainter& p) {
    if(enabled()) {
        setFontColor(eFontColor::whiteBlack);
        if(hovered()) {
            SDL_Color col1;
            SDL_Color col2;
            eFontColorHelpers::colors(eFontColor::whiteBlack, col1, col2);
            const int w = lineWidth();

            const SDL_Rect rect2{0, height() - w, width(), w};
            p.fillRect(rect2, col2);

            const int dx = w;
            const int dy = w;
            const SDL_Rect rect1{dx, height() - w + dy, width(), w};
            p.fillRect(rect1, col1);
        }
    } else {
        setFontColor(eFontColor::grayBlack);
    }
    return eButtonBase::paintEvent(p);
}

eESCMenuSwitchButton::eESCMenuSwitchButton(
    const std::string& mainText,
    const std::vector<std::string>& values,
    const int iniId,
    const eSwitchAction& switchA,
    eMainWindow* const window,
    const int width) :
    eESCMenuButton("", window) {
    setNoPadding();
    setWidth(width);

    const auto main = new eLabel(window);
    main->setHugeFontSize();
    main->setFontColor(eFontColor::whiteBlack);
    main->setText(mainText);
    main->fitContent();
    addWidget(main);

    mValue = new eLabel(window);
    mValue->setHugeFontSize();
    mValue->setFontColor(eFontColor::whiteBlack);
    mId = iniId;
    mValue->setText(values[mId]);
    mValue->fitContent();
    addWidget(mValue);
    mValue->align(eAlignment::right);

    setPressAction([this, values, switchA]() {
        mId = (mId + 1) % values.size();
        switchA(mId);
        mValue->setText(values[mId]);
        mValue->fitContent();
        mValue->align(eAlignment::right);
    });

    fitHeight();
}
