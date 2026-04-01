#include "elabelbase.h"

#include "../emainwindow.h"

#include "../textures/etextgenerator.h"

#include <algorithm>

eLabelBase::eLabelBase(eMainWindow* const window) :
    mWindow(window) {
    const auto& res = eLabelBase::res();
    mFont = eFonts::defaultFont(res);
}

bool eLabelBase::setTinyFontSize() {
    const int s = res().tinyFontSize();
    return setFontSize(s);
}

bool eLabelBase::setVerySmallFontSize() {
    const int s = res().verySmallFontSize();
    return setFontSize(s);
}

bool eLabelBase::setSmallFontSize() {
    const int s = res().smallFontSize();
    return setFontSize(s);
}

bool eLabelBase::setLargeFontSize() {
    const int s = res().largeFontSize();
    return setFontSize(s);
}

bool eLabelBase::setHugeFontSize() {
    const int s = res().hugeFontSize();
    return setFontSize(s);
}

bool eLabelBase::setExtraHugeFontSize() {
    const int s = res().extraHugeFontSize();
    return setFontSize(s);
}

bool eLabelBase::setFontSize(const int s) {
    mFont.fPtSize = s;
    return updateTextTexture();
}

bool eLabelBase::setFont(const eFont& font) {
    mFont = font;
    return updateTextTexture();
}

bool eLabelBase::updateTexture() {
    if(!mText.empty()) {
        updateTextTexture();
        return true;
    } else {
        return false;
    }
}

bool eLabelBase::setText(const std::string& text) {
    if(text == mText) return true;
    mText = text;
    return updateTextTexture();
}

bool eLabelBase::setTexture(const std::shared_ptr<eTexture>& tex) {
    mTexture = tex;
    return true;
}

bool eLabelBase::setFontColor(const eFontColor color) {
    if(mFontColor == color) return true;
    mFontColor = color;
    return updateTextTexture();
}

int eLabelBase::fontSize() const {
    return mFont.fPtSize;
}

void eLabelBase::setWrapWidth(const int w) {
    mWidth = w;
    updateTextTexture();
}

bool eLabelBase::updateTextTexture() {
    if(mText.empty()) {
        mTexture.reset();
        return true;
    }
    const auto r = mWindow->renderer();
    const int lineWidth = eLabelBase::lineWidth();
    const eTextGenerator textGenerator(r, mFontColor, mFont,
                                       lineWidth, mWidth);
    mTexture = textGenerator.generate(mText);
    if(!mTexture) {
        mTexture.reset();
        mUpdateTextTextureFailed = true;
        return false;
    }
    return true;
}

const eResolution& eLabelBase::res() const {
    return mWindow->resolution();
}

int eLabelBase::lineWidth() const {
    return std::max(1, fontSize()/15);
}

void eLabelBase::textureSize(int& w, int& h) const {
    if(!mTexture) {
        w = 0;
        h = 0;
    } else {
        w = mTexture->width();
        h = mTexture->height();
    }
}

const std::shared_ptr<eTexture>& eLabelBase::texture() {
    if(mUpdateTextTextureFailed) {
        mUpdateTextTextureFailed = false;
        updateTextTexture();
    }
    return mTexture;
}
