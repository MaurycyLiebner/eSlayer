#include "emainmenubutton.h"

eMainMenuButton::eMainMenuButton(const std::string& text,
                                 eMainWindow * const window) :
    eButtonBase(window) {
    setFontColor(eFontColor::white);
    setText(text);
    fitContent();
}

void eMainMenuButton::paintEvent(ePainter& p) {
    if(enabled()) {
        setFontColor(eFontColor::white);
        if(hovered()) {
            const SDL_Color white{255, 255, 255, 255};
            p.drawRect(rect(), white, lineWidth());
        }
    } else {
        setFontColor(eFontColor::gray);
    }
    return eButtonBase::paintEvent(p);
}
