#ifndef EITEMINSTANCETEXTURE_H
#define EITEMINSTANCETEXTURE_H

#include "etexture.h"
#include "etexturecolorholder.h"

#include <eSlayerHelpers/eitem.h>
#include <eSlayerHelpers/epoint.h>

class eResolution;

class eItemInstanceTexture {
public:
    eItemInstanceTexture();
    eItemInstanceTexture(SDL_Renderer* const r,
                         const eResolution& res,
                         const eItem& item);
    eTextureColorHolder request() const;
    eTextureColorHolder requestJewel(const uint8_t id) const;
    uint8_t nJewels() const { return mJewels.size(); }
    uint8_t nSockets() const { return mNSockets; }
    void reset();

    static ePointF jewelPosition(
        const uint8_t id, const uint8_t nSockets);
    static SDL_FColor color(const eItemBase& item);

    explicit operator bool() const {
        return !!mTex;
    }
private:
    uint8_t mNSockets = 0;
    std::vector<eItemInstanceTexture> mJewels;
    std::shared_ptr<eTexture> mTex;
    eTextureColorSetting mColor;
};

#endif // EITEMINSTANCETEXTURE_H
