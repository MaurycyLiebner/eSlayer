#ifndef ETERRSTEXTURES_H
#define ETERRSTEXTURES_H

#include "etilestextures.h"

class eTerrsTextures : public eTilesTextures {
public:
    eTerrsTextures();

    static eTileTextures* get(const std::string& name);
    static void load();
private:
    static eTerrsTextures sInstance;
};

#endif // ETERRSTEXTURES_H
