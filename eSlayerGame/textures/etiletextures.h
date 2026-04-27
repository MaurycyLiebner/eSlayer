#ifndef ETILETEXTURES_H
#define ETILETEXTURES_H

#include "../textures/etexturecollection.h"

#include <eSlayerHelpers/eterrstexturesdata.h>

class eResolution;

class eTileTextures : public eTileTextureData {
    friend class eTerrsTextures;
public:
    const std::shared_ptr<eTexture>& getTexture(const int id) const;
    int size() const;

    void load(const int w, const int h,
              const eResolution& res,
              SDL_Renderer * const r);
private:
    void load(const eResolution& res,
              SDL_Renderer * const r);
    void loadFixedSize(const int w, const int h,
                       const eResolution& res,
                       SDL_Renderer * const r);

    bool mLoaded = false;
    std::shared_ptr<eTextureCollection> mColl;
};

#endif // ETILETEXTURES_H
