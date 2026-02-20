#ifndef EGAMESCREEN_H
#define EGAMESCREEN_H

#include "escreenbase.h"

#include "../textures/echarunitmodel.h"
#include "../textures/elightingtexture.h"

#include <eSlayerMapGenerator/emapgenerator.h>

using eMap = eSlayerMapGenerator::eMap;

class eGameScreen : public eScreenBase {
public:
    using eScreenBase::eScreenBase;

    void initialize(const std::shared_ptr<eMap>& map);
protected:
    void paintEvent(ePainter& p) override;
private:
    void initializeTextures();

    std::shared_ptr<eTexture> mBaseTex;
    std::shared_ptr<eLightingTexture> mLightingTex;
    std::shared_ptr<eTexture> mDisplayTex;

    eCharUnitModel mModel;
    std::shared_ptr<eMap> mMap;
    int mFrame = 0;

    double mPosX = 0.;
    double mPosY = 0.;
};

#endif // EGAMESCREEN_H
