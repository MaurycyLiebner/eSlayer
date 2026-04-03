#ifndef ETERRSTEXTURES_H
#define ETERRSTEXTURES_H

#include "etiletextures.h"

#include <eSlayerHelpers/estringidmapvector.h>

class eTerrsTextures {
public:
    static int id(const std::string& name);
    static eTileTextures& get(const std::string& name);
    static eTileTextures& get(const int id);
    static void load();
private:
    static eStringIdMapVector<eTileTextures> sInstance;
};

#endif // ETERRSTEXTURES_H
