#include "echeckbutton.h"

void eCheckButton::paintEvent(ePainter& p) {
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
