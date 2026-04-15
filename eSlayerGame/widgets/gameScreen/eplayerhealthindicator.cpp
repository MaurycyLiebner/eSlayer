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
    setHeight(mFg->height());
    mNColumns = nColumns;
    setWidth(nColumns*mFg->width());
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
    const int baseWidth = mFg->width();
    for(int x = 0; x < mNColumns; x++) {
        p.drawTexture(x*baseWidth, 0, mFg);
    }
    const auto& col = color();
    if(col.r + col.g + col.b > 600) {
        mBg->setColorMod(55, 55, 55);
    }
    const int xOffset = mBg->x();
    for(int x = 0; x < mNColumns; x++) {
        p.drawTexture(x*baseWidth + xOffset, 0, mBg);
    }
    mBg->setColorMod(col.r, col.g, col.b);
    const int innerW = mBg->width();
    const int totalW = mNColumns*innerW;
    const float frac = float(value)/max;
    const int h = height();
    int remW = frac*totalW;
    for(int x = 0; x < mNColumns; x++) {
        const SDL_Rect rect{x*baseWidth + xOffset, 0, remW, h};
        p.setClipRect(&rect);
        p.drawTexture(rect.x, 0, mBg);
        remW -= innerW;
        if(remW <= 0) break;
    }
    p.setClipRect(nullptr);
    mBg->clearColorMod();
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
