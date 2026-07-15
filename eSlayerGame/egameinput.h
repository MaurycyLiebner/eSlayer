#ifndef EGAMEINPUT_H
#define EGAMEINPUT_H

#include <eSlayerHelpers/epoint.h>

class eGameInput {
public:
    eGameInput(const int tileW, const int tileH);

    int tileWidth() const { return mTileW; }
    int tileHeight() const { return mTileH; }

    ePointF pixelToTilePos(const ePointF& charPos,
                           const ePointF& pixel,
                           const int screenW,
                           const int screenH) const;

    ePointF pixelToTilePos(const ePointF& charPos,
                           const int screenW,
                           const int screenH) const;

    ePointF tilePosToPixel(const ePointF& pos,
                           const ePointF& charPos,
                           const int screenW,
                           const int screenH) const;

    void handleMousePress(const bool leftPressed,
                          const bool rightPressed,
                          const float x, const float y);

    void handleMouseRelease(const bool leftReleased,
                            const bool rightReleased);

    void handleMouseMove(const float x, const float y);

    bool mousePressed() const { return mMousePressed; }
    bool rightPressed() const { return mRightPressed; }
    bool leftPressed() const { return mLeftPressed; }
    const ePointF& mousePos() const { return mMousePos; }

    const float* characterVerticalPosPtr() const { return &mCharacterVerticalPos; }
    const float* characterHorizontalPosPtr() const { return &mCharacterHorizontalPos; }

    float characterVerticalPos() const { return mCharacterVerticalPos; }
    float characterHorizontalPos() const { return mCharacterHorizontalPos; }
    void setCharacterHorizontalPos(const float pos) { mCharacterHorizontalPos = pos; }
private:
    const int mTileW;
    const int mTileH;

    const float mCharacterVerticalPos = 0.45f;
    float mCharacterHorizontalPos = 0.5f;

    bool mMousePressed = false;
    bool mRightPressed = false;
    bool mLeftPressed = false;
    ePointF mMousePos{0.f, 0.f};
};

#endif // EGAMEINPUT_H
