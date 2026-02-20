#ifndef EGAMESCREEN_H
#define EGAMESCREEN_H

#include "escreenbase.h"

#include "../textures/echarunitmodel.h"
#include "../textures/elightingtexture.h"

#include <eSlayerMapGenerator/emapgenerator.h>

using eMap = eSlayerMapGenerator::eMap;

#include <eSlayerHelpers/epoint.h>

class eGameScreen : public eScreenBase {
public:
    using eScreenBase::eScreenBase;

    void initialize(const std::shared_ptr<eMap>& map);
protected:
    void paintEvent(ePainter& p) override;
    bool mousePressEvent(const eMouseEvent& e) override;
    bool mouseReleaseEvent(const eMouseEvent& e) override;
    bool mouseMoveEvent(const eMouseEvent& e) override;
private:
    void initializeTextures();

    ePointF pixelToTilePos(const ePointF& pixel);
    void updateTargetPos();
    void setTargetPos(const ePointF& pos);

    std::shared_ptr<eTexture> mBaseTex;
    std::shared_ptr<eLightingTexture> mLightingTex;
    std::shared_ptr<eTexture> mDisplayTex;

    eCharUnitModel mModel;
    std::shared_ptr<eMap> mMap;
    int mFrame = 0;

    const int mTileW = 160;
    const int mTileH = 79;

    ePointF mPos{0., 0.};
    bool mMousePressed = false;
    ePointF mMousePos{0., 0.};
};

#endif // EGAMESCREEN_H
