#include "echeckbox.h"

void eCheckBox::sizeHint(int& w, int& h) {
    const auto res = resolution();
    const int p = res.smallFontSize();
    w = p;
    h = p;
}

void eCheckBox::paintEvent(ePainter& p) {
    const SDL_Color white{255, 255, 255, 255};
    p.drawRect(rect(), white, lineWidth());
    if(checked()) {
        auto r = rect();
        {
            const auto res = resolution();
            const int p = r.w/4;
            r.x += p;
            r.y += p;
            r.w -= 2*p;
            r.h -= 2*p;
        }
        p.fillRect(r, white);
    }
}
