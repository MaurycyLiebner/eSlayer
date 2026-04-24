#ifndef ETEXTURECOLLECTION_H
#define ETEXTURECOLLECTION_H

#include "etexture.h"

#include <vector>

class eTextureCollection {
public:
    std::shared_ptr<eTexture>& addTexture();
    void addTexture(const std::shared_ptr<eTexture>& tex);

    const std::shared_ptr<eTexture>& getTexture(const int id) const;
    int size() const;
private:
    std::vector<std::shared_ptr<eTexture>> mTexs;
};

#endif // ETEXTURECOLLECTION_H
