#include "eslider.h"

#include <algorithm>

void eSlider::setSetter(const eSetter& setter) {
    mSetter = setter;
}

bool eSlider::mousePressEvent(const eMouseEvent& e) {
    const bool left = e.button() == eMouseButton::left;
    if(!left) return false;
    triggerX(e.x());
    return true;
}

bool eSlider::mouseMoveEvent(const eMouseEvent& e) {
    const auto leftM = e.buttons() & eMouseButton::left;
    const bool left = static_cast<bool>(leftM);
    if(!left) return false;
    triggerX(e.x());
    return true;
}

void eSlider::triggerX(const int x) {
    const int w = width();
    const int max = eSlider::max();
    const int v = std::clamp((x*max + w/2)/w, 0, max);
    setValue(v);
    if(mSetter) mSetter(v);
}
