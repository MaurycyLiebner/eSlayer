#include "escrollwidget.h"

#include <algorithm>

void eScrollWidget::initializeButtons() {
    clampDY();
}

void eScrollWidget::setScrollArea(eWidget* const w) {
    setMouseReceiver(w);
    mScrollArea = w;
    clampDY();
}

void eScrollWidget::scrollUp() {
    mDy -= mStep;
    clampDY();
}

void eScrollWidget::scrollDown() {
    mDy += mStep;
    clampDY();
}

void eScrollWidget::setDY(const int dy) {
    mDy = dy;
    clampDY();
}

void eScrollWidget::scrollToTheTop() {
    mDy = 0;
    clampDY();
}

void eScrollWidget::scrollToTheBottom() {
    mDy = 100000;
    clampDY();
}

void eScrollWidget::paintEvent(ePainter& p) {
    if(mScrollArea) {
        const auto r = rect();
        p.setClipRect(&r);

        p.translate(0, -mDy);
        mScrollArea->paint(p);
        p.translate(0, mDy);

        p.setClipRect(nullptr);
    }
}

bool eScrollWidget::keyPressEvent(const eKeyPressEvent& e) {
    if(e.key() == SDL_SCANCODE_UP) {
        scrollUp();
        return true;
    } else if(e.key() == SDL_SCANCODE_DOWN) {
        scrollDown();
        return true;
    }
    if(!mScrollArea) return false;
    return mScrollArea->keyPress(e.translated(0, -mDy));
}

bool eScrollWidget::mouseWheelEvent(const eMouseWheelEvent& e) {
    mDy -= mStep*e.dy();
    clampDY();
    return true;
}

void eScrollWidget::clampDY() {
    if(mScrollArea) {
        const int sh = mScrollArea->height();
        const int h = height();
        const int maxH = sh - h;
        if(h > sh) {
            mDy = 0;
        } else {
            mDy = std::clamp(mDy, 0, maxH);
        }
        setMouseReceiverDXDY(0, mDy);
    } else {
        mDy = 0;
    }
}

void eScrollWidget::setScrollStep(const int s) {
    mStep = s;
}
