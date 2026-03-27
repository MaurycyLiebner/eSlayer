#include "etexturecheckbutton.h"

void eTextureCheckButton::initialize(
    const std::shared_ptr<eTexture>& checked,
    const std::shared_ptr<eTexture>& unchecked) {
    mChecked = checked;
    mUnchecked = unchecked;
    setTexture(checked);
    setNoPadding();
    fitContent();
}

void eTextureCheckButton::paintEvent(ePainter& p) {
    if(checked()) {
        p.drawTexture(0, 0, mChecked);
    } else {
        p.drawTexture(0, 0, mUnchecked);
    }
}
