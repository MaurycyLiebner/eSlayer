#include "eplayerhealthindicator.h"

void ePlayerHealthIndicator::setName(const std::string& name) {
    mName = name;
}

void ePlayerHealthIndicator::paintEvent(ePainter& p) {
    if(mHovered) {
        const int value = eProgressBar::value();
        const int max = eProgressBar::max();
        setText(mName + ": " + std::to_string(value) + " / " + std::to_string(max));
    }
    eHealthIndicator::paintEvent(p);
}

bool ePlayerHealthIndicator::mouseMoveEvent(const eMouseEvent& e) {
    (void)e;
    return true;
}

bool ePlayerHealthIndicator::mouseEnterEvent(const eMouseEvent& e) {
    mHovered = true;
    showText();
    return true;
}

bool ePlayerHealthIndicator::mouseLeaveEvent(const eMouseEvent& e) {
    mHovered = false;
    hideText();
    return true;
}
