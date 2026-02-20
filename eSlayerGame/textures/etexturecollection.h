#ifndef ETEXTURECOLLECTION_H
#define ETEXTURECOLLECTION_H

#include "etexture.h"

class eTextureCollection {
public:
    eTextureCollection(SDL_Renderer* const r);

    std::shared_ptr<eTexture>& addTexture();
    void addTexture(const std::shared_ptr<eTexture>& tex);

    const std::shared_ptr<eTexture>& getTexture(const int id) const;
    int size() const;
private:
    SDL_Renderer* const mRenderer;
    std::vector<std::shared_ptr<eTexture>> mTexs;
};

#endif // ETEXTURECOLLECTION_H
