#ifndef EOBJSTEXTURES_H
#define EOBJSTEXTURES_H

#include <eSlayerHelpers/estringidmapvector.h>

#include "etexturecollection.h"

class eResolution;

struct eObjAnim {
    eObjAnim(const int frames) :
        fFrames(frames) {}

    int fFrames;
    eTextureCollection fTexs;
};

struct eObjTextures {
    std::string fName;

    using eType = std::vector<eObjAnim>;
    bool fBlocksLight;
    bool fShadow;
    bool fFlat;
    float fLightRadius;
    std::vector<eType> fTypes;
    std::map<std::string, int> fAnimIds;

    void load(const eResolution& res,
              SDL_Renderer * const r);
private:
    bool mLoaded = false;
};

class eObjsTextures {
public:
    static int id(const std::string& name);
    static eObjTextures& get(const std::string& name);
    static eObjTextures& get(const int id);
    static void load();
private:
    static bool sLoaded;
    static eStringIdMapVector<eObjTextures> sInstance;
};

#endif // EOBJSTEXTURES_H
