#include "etexturecollection.h"

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
