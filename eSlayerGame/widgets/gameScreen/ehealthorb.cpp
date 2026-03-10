#include "ehealthorb.h"

#include "../../textures/euitextures.h"

#include <cmath>

void eHealthOrb::initialize() {
    const auto& orb = eUITextures::sOrb;
    setTexture(orb.getTexture(0));
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
    const auto& orb = eUITextures::sOrb;
    const auto& orbBack = orb.getTexture(1);
    orbBack->setColorMod(mColor.r, mColor.g, mColor.b);
    orbBack->setAlpha(mColor.a);
    const int wTotal = width();
    const int hTotal = height();
    const int hBase = orbBack->height();
    const int y = (hTotal - hBase)/2;
    const int h = std::round(double(hBase)*mValue/mMax);
    const SDL_Rect clipRect{0, y + hBase - h, wTotal, h};
    p.setClipRect(&clipRect);
    const int cx = wTotal/2;
    const int cy = hTotal/2;
    p.drawTexture(cx, cy, orbBack, eAlignment::center);
    p.setClipRect(nullptr);
    orbBack->clearColorMod();
    orbBack->clearAlphaMod();
    eLabel::paintEvent(p);
}
