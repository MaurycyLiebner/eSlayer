#include "../include/eSlayerHelpers/epathfindermap.h"

ePathFinderMap::ePathFinderMap() {}

ePathFinderMap::ePathFinderMap(const int x, const int y,
                               const int w, const int h) :
    mX(x), mY(y), mWidth(w), mHeight(h) {
    resize(h);
    for(int y = 0; y < h; y++) {
        operator[](y).resize(w);
    }
}

void ePathFinderMap::set(const ePoint& p, const bool v) {
    operator[](p.fY - mY)[p.fX - mX] = v;
}

bool ePathFinderMap::get(const ePoint& p) const {
    const int x = p.fX - mX;
    const int y = p.fY - mY;
    if(x < 0 || x >= mWidth) return false;
    if(y < 0 || y >= mHeight) return false;
    return operator[](y)[x];
}
