#include "eplayerhealthindicator.h"

#include "../../textures/euitextures.h"

void ePlayerHealthIndicator::initialize(
    const std::shared_ptr<eTexture>& bg,
    const std::shared_ptr<eTexture>& fg,
    const int nColumns) {
    mBg = bg;
    mFg = fg;
    eHealthIndicator::initialize();
    setNoPadding();
    setHeight(mBg->height());
    setWidth(nColumns*mBg->width());
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
    if(col.r + col.g + col.b > 600) {
        mBg->setColorMod(55, 55, 55);
    }
    for(int x = 0; x < width(); x += mBg->width()) {
        p.drawTexture(x, 0, mBg);
    }
    mBg->setColorMod(col.r, col.g, col.b);
    const float frac = float(value)/max;
    const int w = frac*width();
    const SDL_Rect rect{0, 0, w, height()};
    p.setClipRect(&rect);
    for(int x = 0; x < width(); x += mBg->width()) {
        p.drawTexture(x, 0, mBg);
    }
    p.setClipRect(nullptr);
    mBg->clearColorMod();
    if(mFg) p.drawTexture(0, 0, mFg);
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
