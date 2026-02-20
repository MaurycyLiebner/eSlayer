#ifndef EEFFECTSTEXTURES_H
#define EEFFECTSTEXTURES_H

#include "eeffecttextures.h"

class eEffectsTextures {
public:
    eEffectsTextures();

    static eEffectTextures* get(const std::string& name);
    static void load();
private:
    void loadImpl();
    eEffectTextures* getImpl(const std::string& name);

    static eEffectsTextures sInstance;

    bool mLoaded = false;
    std::map<std::string, eEffectTextures> mTerrs;
};

#endif // EEFFECTSTEXTURES_H
