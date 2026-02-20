#ifndef ETERRTEXTURES_H
#define ETERRTEXTURES_H

#include "../textures/etexturecollection.h"

#include <string>

class eTerrTextures {
public:
    eTerrTextures();

    const std::shared_ptr<eTexture>& getTexture(const int id) const;
    int size() const;

    void load(SDL_Renderer * const r);

    void setName(const std::string& name) { mName = name; }
private:
    bool mLoaded = false;
    std::string mName;
    std::shared_ptr<eTextureCollection> mColl;
};

#endif // ETERRTEXTURES_H
