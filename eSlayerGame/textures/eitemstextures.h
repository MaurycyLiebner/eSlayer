#ifndef EITEMSTEXTURES_H
#define EITEMSTEXTURES_H

#include <eSlayerHelpers/estringidmapvector.h>
#include <eSlayerHelpers/eitemdata.h>

#include "etexture.h"

struct eItemTexture {
    int fItemDataId;
    std::string fTexPath;
    std::shared_ptr<eTexture> fTex;

    void request(SDL_Renderer* const r);
};

class eItemsTextures {
public:
    static eItemTexture& get(const std::string& name);
    static eItemTexture& get(const int id);
    static void load();
private:
    void loadImpl();
    eItemTexture& getImpl(const std::string& name);
    eItemTexture& getImpl(const int id);

    static eItemsTextures sInstance;

    bool mLoaded = false;
    eStringIdMapVector<eItemTexture> mTexs;
};

#endif // EITEMSTEXTURES_H
