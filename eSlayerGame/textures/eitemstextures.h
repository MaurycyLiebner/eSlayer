#ifndef EITEMSTEXTURES_H
#define EITEMSTEXTURES_H

#include "etexture.h"

#include <eSlayerHelpers/estringidmapvector.h>
#include <eSlayerHelpers/eitemdata.h>

#include <nlohmann/json.hpp>
using namespace nlohmann;

struct eItemTexture {
    int fItemDataId;
    std::string fTexPath;
    std::shared_ptr<eTexture> fTex;

    void request(SDL_Renderer* const r,
                 const eResolution& res);
};

class eItemsTextures {
public:
    static eItemTexture& get(const std::string& name);
    static eItemTexture& get(const int id);
    static eItemTexture& getByItemDataId(const int itemDataId);
    static void load();
private:
    void loadImpl();
    void loadImpl(const std::string& name,
                  const std::string& path,
                  const int width,
                  const int height);
    eItemTexture& getImpl(const std::string& name);
    eItemTexture& getImpl(const int id);
    eItemTexture& getByItemDataIdImpl(const int itemDataId);

    static eItemsTextures sInstance;

    bool mLoaded = false;
    eStringIdMapVector<eItemTexture> mTexs;
    std::map<int, int> mItemDataIdToTexId;
};

#endif // EITEMSTEXTURES_H
