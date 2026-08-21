#include "egameinput.h"

eGameInput::eGameInput(const int tileW, const int tileH) :
    mTileW(tileW), mTileH(tileH) {}

ePointF eGameInput::pixelToTilePos(
    const ePointF& charPos, const ePointF& pixel,
    const int screenW, const int screenH) const {
    ePointF result;
    result.fY = charPos.fY +
                (pixel.fY - mCharacterVerticalPos*screenH)/mTileH +
                (mCharacterHorizontalPos*screenW - pixel.fX)/mTileW;
    result.fX = charPos.fX +
                (pixel.fX - mCharacterHorizontalPos*screenW)/mTileW +
                (pixel.fY - mCharacterVerticalPos*screenH)/mTileH;
    return result;
}

ePointF eGameInput::pixelToTilePos(
    const ePointF& charPos,
    const int screenW,
    const int screenH) const {
    return pixelToTilePos(charPos, mMousePos, screenW, screenH);
}

ePointF eGameInput::tilePosToPixel(
    const ePointF& pos,
    const ePointF& charPos,
    const int screenW,
    const int screenH) const {
    ePointF result;
    result.fY = mCharacterVerticalPos*screenH +
                (pos.fY - charPos.fY + pos.fX - charPos.fX)*((mTileH + 1)/2);
    result.fX = mCharacterHorizontalPos*screenW +
                (charPos.fY - pos.fY + pos.fX - charPos.fX)*(mTileW/2);
    return result;
}

void eGameInput::handleMousePress(
    const bool leftPressed,
    const bool rightPressed,
    const float x, const float y) {
    if(rightPressed) mRightPressed = true;
    if(leftPressed) mLeftPressed = true;
    mMousePressed = true;
    mMousePos = ePointF{x, y};
}

void eGameInput::handleMouseRelease(
    const bool leftReleased,
    const bool rightReleased) {
    if(rightReleased) mRightPressed = false;
    if(leftReleased) mLeftPressed = false;
    mMousePressed = mRightPressed || mLeftPressed;
}

void eGameInput::handleMouseMove(const float x, const float y) {
    mMousePos = ePointF{x, y};
}
