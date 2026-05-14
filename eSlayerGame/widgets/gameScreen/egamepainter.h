#ifndef EGAMEPAINTER_H
#define EGAMEPAINTER_H

#include "../epainter.h"
#include "../../textures/elightinghandler.h"

class eGamePainter : public ePainter {
public:
    using ePainter::ePainter;

    std::shared_ptr<eTexture>
    initialize(const int w, const int h,
               const int tileW, const int tileH);

    eRenderTargetHolder switchToBase();
    eRenderTargetHolder switchToItemNames();

    void setLightness(const Uint8 light);
    void setContrast(const Uint8 cont)
    { mContrast = cont; }

    void clear();
    void addLight(const float tx, const float ty,
                  const float radius);
    void finish(const float tx0, const float ty0,
                const eResolution& res);

    void addRenderCall(const eRenderCallType type,
                       const float tx, const float ty,
                       const float px, const float py,
                       const std::shared_ptr<eTexture>& tex);

    void addObjectShadow(const float tx, const float ty,
                         const float size);
    void addWallShadow(const int tx, const int ty,
                       const eWallType dir,
                       const float wallMin,
                       const float wallMax);
private:
    std::vector<eLight> mLights;
    bool mRenderItemNames = false;
    Uint8 mLight = 180;
    Uint8 mContrast = 140;
    std::shared_ptr<eTexture> mItemNames;
    std::shared_ptr<eTexture> mBaseTex;
    std::shared_ptr<eLightingHandler> mLightingTex;
    std::shared_ptr<eTexture> mDisplayTex;
};

#endif // EGAMEPAINTER_H
