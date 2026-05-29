#ifndef EITEMINSTANCETEXTURE_H
#define EITEMINSTANCETEXTURE_H

#include "etexture.h"
#include "etexturecolorholder.h"

#include <eSlayerHelpers/eitem.h>

class eResolution;

class eItemInstanceTexture {
public:
    eItemInstanceTexture();
    eItemInstanceTexture(SDL_Renderer* const r,
                         const eResolution& res,
                         const eItem& item);
    eTextureColorHolder request() const;
    void reset();

    static SDL_FColor color(const eItemBase& item);

    explicit operator bool() const {
        return !!mTex;
    }
private:
    std::shared_ptr<eTexture> mTex;
    eTextureColorSetting mColor;
};

#endif // EITEMINSTANCETEXTURE_H
