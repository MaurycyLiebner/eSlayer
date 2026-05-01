#ifndef EGAMEPAINTER_H
#define EGAMEPAINTER_H

#include "../epainter.h"
#include "../../textures/elightingtexture.h"

class eGamePainter : public ePainter {
public:
    using ePainter::ePainter;

    std::shared_ptr<eTexture>
    initialize(const int w, const int h);

    eRenderTargetHolder switchToLighting();
    eRenderTargetHolder switchToBase();
    eRenderTargetHolder switchToItemNames();

    void setLightness(const Uint8 light);
    void setContrast(const Uint8 cont)
    { mContrast = cont; }

    void clear();
    void renderLight(const float tx, const float ty,
                     const float x, const float y,
                     const float radius,
                     const SDL_Color& color,
                     const ePaintCall& paintCall);
    void finish(const eResolution& res);

    void addObjectShadow(const float px, const float py,
                         const float tileCenterY,
                         const float size,
                         const std::shared_ptr<eTexture>& tex);
    void addWallShadow(const int tx, const int ty,
                       const float px, const float py,
                       const eWallType dir,
                       const int tileW,
                       const int tileH,
                       const std::shared_ptr<eTexture>& tex);
private:
    std::vector<eLightBlocker> mLightBlockers;
    std::vector<eWallLightBlocker> mWallLightBlockers;
    std::vector<eLight> mLights;
    bool mRenderItemNames = false;
    Uint8 mLight = 180;
    Uint8 mContrast = 140;
    std::shared_ptr<eTexture> mItemNames;
    std::shared_ptr<eTexture> mBaseTex;
    std::shared_ptr<eLightingTexture> mLightingTex;
    std::shared_ptr<eTexture> mDisplayTex;
};

#endif // EGAMEPAINTER_H
