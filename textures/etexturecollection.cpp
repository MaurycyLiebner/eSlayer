#include "etexturecollection.h"

eTextureCollection::eTextureCollection(SDL_Renderer* const r) :
    mRenderer(r) {}

std::shared_ptr<eTexture>& eTextureCollection::addTexture() {
    return mTexs.emplace_back(std::make_shared<eTexture>());
}

void eTextureCollection::addTexture(const std::shared_ptr<eTexture>& tex) {
    mTexs.push_back(tex);
}

const std::shared_ptr<eTexture>&
eTextureCollection::getTexture(const int id) const {
    return mTexs[id];
}

int eTextureCollection::size() const {
    return mTexs.size();
}
