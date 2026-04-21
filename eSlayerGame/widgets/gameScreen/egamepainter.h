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
    void renderLight(SDL_Renderer * const r,
                     const float x, const float y,
                     const float radius,
                     const SDL_Color& color);
    void finish();
private:
    bool mRenderItemNames = false;
    Uint8 mLight = 180;
    Uint8 mContrast = 140;
    std::shared_ptr<eTexture> mItemNames;
    std::shared_ptr<eTexture> mBaseTex;
    std::shared_ptr<eLightingTexture> mLightingTex;
    std::shared_ptr<eTexture> mDisplayTex;
};

#endif // EGAMEPAINTER_H
