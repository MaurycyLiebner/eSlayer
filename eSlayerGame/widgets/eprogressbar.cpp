#include "eprogressbar.h"

void eProgressBar::setRange(const int min, const int max) {
    mMin = min;
    mMax = max;
}

void eProgressBar::setValue(const int v) {
    mValue = v;
}

void eProgressBar::paintEvent(ePainter& p) {
    const float per = float(mValue - mMin)/(mMax - mMin);
    const SDL_Rect baseRect = rect();
    const int pp = padding();
    const SDL_Rect innerRect{pp, pp,
                             baseRect.w - 2*pp,
                             baseRect.h - 2*pp};
    const int w = per*innerRect.w;
    const SDL_Rect rect{innerRect.x, innerRect.y,
                        w, innerRect.h};
    p.fillRect(rect, {255, 255, 255, 255});
    p.drawRect(baseRect, {255, 255, 255, 255}, lineWidth());
}
