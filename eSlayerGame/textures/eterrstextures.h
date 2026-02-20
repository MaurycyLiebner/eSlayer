#ifndef ETERRSTEXTURES_H
#define ETERRSTEXTURES_H

#include "eterrtextures.h"

class eTerrsTextures {
public:
    eTerrsTextures();

    static eTerrTextures* get(const std::string& name);
    static void load();
private:
    void loadImpl();
    eTerrTextures* getImpl(const std::string& name);

    static eTerrsTextures sInstance;

    bool mLoaded = false;
    std::map<std::string, eTerrTextures> mTerrs;
};

#endif // ETERRSTEXTURES_H
