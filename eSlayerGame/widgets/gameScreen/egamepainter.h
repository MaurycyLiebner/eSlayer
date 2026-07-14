#ifndef EGAMEPAINTER_H
#define EGAMEPAINTER_H

#include "../epainter.h"
#include "../../textures/elightinghandler.h"
#include "effects/eeffect.h"

class eGamePainter : public ePainter {
public:
    eGamePainter(eTilesIterator& iterator,
                 SDL_Renderer* const r);

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
    void finish(const eResolution& res);

    void calculateAndRenderLighting();

    void render(const eRenderCall& c);

    void addObjectShadow(const float tx, const float ty,
                         const float size);
    void addRectShadow(const float tx, const float ty,
                       const float width, const float height);
    void addWallShadow(const int tx, const int ty,
                       const eWallType dir,
                       const float wallMin,
                       const float wallMax,
                       const bool minFeatherForce,
                       const bool maxFeatherForce);

    void applyEffects();
private:
    std::vector<eLight> mLights;
    bool mRenderItemNames = false;
    Uint8 mLight = 180;
    Uint8 mContrast = 140;
    std::shared_ptr<eTexture> mItemNames;
    std::shared_ptr<eTexture> mBaseTex;
    std::shared_ptr<eTexture> mEffectTex;
    eLightingHandler mLightingTex;
    std::shared_ptr<eTexture> mDisplayTex;
    eEffects mEffects;
};

#endif // EGAMEPAINTER_H
