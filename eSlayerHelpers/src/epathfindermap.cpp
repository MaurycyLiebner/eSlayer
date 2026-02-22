#include "../include/eSlayerHelpers/epathfindermap.h"

ePathFinderMap::ePathFinderMap() {}

ePathFinderMap::ePathFinderMap(const int w, const int h) :
    mWidth(w), mHeight(h) {
    resize(h);
    for(int y = 0; y < h; y++) {
        operator[](y).resize(w);
    }
}

void ePathFinderMap::set(const ePoint& p, const bool v) {
    operator[](p.fY)[p.fX] = v;
}

bool ePathFinderMap::get(const ePoint& p) const {
    if(p.fX < 0 || p.fX >= mWidth) return false;
    if(p.fY < 0 || p.fY >= mHeight) return false;
    return operator[](p.fY)[p.fX];
}
