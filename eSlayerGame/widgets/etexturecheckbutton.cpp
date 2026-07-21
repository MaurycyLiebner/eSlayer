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
    const bool c = checked();
    const bool h = hovered();
    const auto& tex = c ? mChecked : mUnchecked;
    if(!tex) return;
    p.drawTexture(0, 0, tex);
    if(h) {
        tex->setBlendMode(SDL_BLENDMODE_ADD);
        tex->setAlpha(128);
        p.drawTexture(0, 0, tex);
        tex->setBlendMode(SDL_BLENDMODE_BLEND);
        tex->clearAlphaMod();
    }
}
