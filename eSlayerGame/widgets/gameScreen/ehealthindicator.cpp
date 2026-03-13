#include "ehealthindicator.h"

void eHealthIndicator::initialize() {
    mText = new eLabel(window());
    mText->setFontColor(eFontColor::white);
    addWidget(mText);
}

void eHealthIndicator::setColor(const SDL_Color& col) {
    mColor = col;
}

void eHealthIndicator::showText() {
    mText->show();
}

void eHealthIndicator::hideText() {
    mText->hide();
}

void eHealthIndicator::paintEvent(ePainter& p) {
    const double per = double(value() - min())/(max() - min());
    const SDL_Rect baseRect = rect();
    const int pp = padding();
    const SDL_Rect innerRect{pp, pp,
                             baseRect.w - 2*pp,
                             baseRect.h - 2*pp};
    const int w = per*innerRect.w;
    const SDL_Rect rect{innerRect.x, innerRect.y,
                        w, innerRect.h};
    p.fillRect(baseRect, {0, 0, 0, 255});
    p.fillRect(rect, mColor);
    p.drawRect(baseRect, {255, 255, 255, 255}, lineWidth());
}

void eHealthIndicator::setText(const std::string& text) {
    mText->setText(text);
    mText->fitContent();
    mText->align(eAlignment::center);
}
