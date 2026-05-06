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

    void load(const eResolution& res,
              SDL_Renderer * const r);
private:
    bool mLoaded = false;
    std::shared_ptr<eTextureCollection> mColl;
};

#endif // ETILETEXTURES_H
