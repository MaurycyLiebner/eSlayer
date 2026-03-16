#include "ehealthindicator.h"

void eHealthIndicator::initialize() {
    mText = new eLabel(window());
    mText->setNoPadding();
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
    const float per = float(value() - min())/(max() - min());
    const SDL_Rect baseRect = rect();
    const int w = per*baseRect.w;
    const SDL_Rect rect{baseRect.x, baseRect.y,
                        w, baseRect.h};
    p.fillRect(rect, mColor);
    p.drawRect(baseRect, {255, 255, 255, 255}, lineWidth());
}

void eHealthIndicator::setText(const std::string& text) {
    mText->setText(text);
    mText->fitContent();
    mText->align(eAlignment::center);
    if(mText->height() > height()) {
        mText->setY(-1.2f*mText->height());
    }
}
