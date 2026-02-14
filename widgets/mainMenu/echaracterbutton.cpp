#include "echaracterbutton.h"

#include "../../echaracter.h"

void eCharacterButton::initialize(const eCharacter& c) {
    setText(c.name());
    fitContent();
}

void eCharacterButton::paintEvent(ePainter& p) {
    if(enabled()) {
        setFontColor(eFontColor::white);
        if(hovered() || checked()) {
            const SDL_Color white{255, 255, 255, 255};
            p.drawRect(rect(), white, lineWidth());
        }
    } else {
        setFontColor(eFontColor::gray);
    }
    return eButtonBase::paintEvent(p);
}
