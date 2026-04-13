#include "eplayerhealthindicator.h"

#include "../../textures/euitextures.h"

void ePlayerHealthIndicator::initialize(
    const std::shared_ptr<eTexture>& bg,
    const std::shared_ptr<eTexture>& fg) {
    mBg = bg;
    mFg = fg;
    eHealthIndicator::initialize();
    setNoPadding();
    setTexture(mFg);
    fitContent();
}

void ePlayerHealthIndicator::setName(const std::string& name) {
    mName = name;
}

void ePlayerHealthIndicator::paintEvent(ePainter& p) {
    const int value = eProgressBar::value();
    const int max = eProgressBar::max();
    if(mHovered || mShowText) {
        setText(mName + ": " + std::to_string(value) + " / " + std::to_string(max));
    }
    const auto& col = color();
    p.drawTexture(0, 0, mBg);
    mBg->setColorMod(col.r, col.g, col.b);
    const float frac = float(value)/max;
    const int w = frac*mBg->width();
    const SDL_Rect rect{0, 0, w, height()};
    p.setClipRect(&rect);
    p.drawTexture(0, 0, mBg);
    p.setClipRect(nullptr);
    mBg->clearColorMod();
    eLabel::paintEvent(p);
}

bool ePlayerHealthIndicator::mouseMoveEvent(const eMouseEvent& e) {
    (void)e;
    return true;
}

bool ePlayerHealthIndicator::mouseEnterEvent(const eMouseEvent& e) {
    mHovered = true;
    updateTextVisibility();
    return true;
}

bool ePlayerHealthIndicator::mouseLeaveEvent(const eMouseEvent& e) {
    mHovered = false;
    updateTextVisibility();
    return true;
}

bool ePlayerHealthIndicator::mousePressEvent(const eMouseEvent& e) {
    mShowText = !mShowText;
    updateTextVisibility();
    return true;
}

void ePlayerHealthIndicator::updateTextVisibility() {
    if(mShowText || mHovered) showText();
    else hideText();
}
