#include "ehealthorb.h"

#include "../../textures/euitextures.h"

#include <cmath>

void eHealthOrb::initialize() {
    setNoPadding();
    setTexture(eUITextures::sOrb);
    fitContent();
}

void eHealthOrb::setColor(const SDL_Color& color) {
    mColor = color;
}

void eHealthOrb::setMax(const int max) {
    mMax = max;
}

void eHealthOrb::setValue(const int v) {
    mValue = v;
}

void eHealthOrb::paintEvent(ePainter& p) {
    eLabel::paintEvent(p);
    const auto& orb = eUITextures::sOrb;
    orb->setColorMod(mColor.r, mColor.g, mColor.b);
    const int wTotal = width();
    const int hTotal = height();
    const int h = std::round(double(hTotal)*mValue/mMax);
    const SDL_Rect clipRect{0, hTotal - h, wTotal, h};
    p.setClipRect(&clipRect);
    p.drawTexture(0, 0, orb);
    p.setClipRect(nullptr);
    orb->clearColorMod();
}
