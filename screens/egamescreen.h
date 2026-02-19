#ifndef EGAMESCREEN_H
#define EGAMESCREEN_H

#include "escreenbase.h"

#include "../textures/echarunitmodel.h"
#include "../textures/elightingtexture.h"

class eGameScreen : public eScreenBase {
public:
    using eScreenBase::eScreenBase;

    void initialize();
protected:
    void paintEvent(ePainter& p) override;
private:
    std::shared_ptr<eTexture> mBaseTex;
    std::shared_ptr<eLightingTexture> mLightingTex;
    std::shared_ptr<eTexture> mDisplayTex;

    eCharUnitModel mModel;
    int mFrame = 0;
};

#endif // EGAMESCREEN_H
