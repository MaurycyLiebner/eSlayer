#ifndef ESPRITELOADER_H
#define ESPRITELOADER_H

#include "etexture.h"

#include <SDL3/SDL_rect.h>

#include <string>
#include <vector>

class eResolution;

class eTextureCollection;

class eSpriteLoader {
public:
    eSpriteLoader(const std::string& dir,
                  const std::string& path,
                  const eResolution& res,
                  SDL_Renderer* const r,
                  const SDL_Color &colorKey = {0, 0, 0, 0});

    std::shared_ptr<eTexture> load(const int i);
    std::shared_ptr<eTexture> load(const int i,
                                   eTextureCollection& coll);
    int loadAll(eTextureCollection& coll);
private:
    void initialize();

    const std::string mDir;
    const std::string mPath;
    const eResolution& mRes;
    SDL_Renderer* const mRenderer;
    const SDL_Color mColorKey = {0, 0, 0, 0};
    bool mInitialized = false;

    struct eCsvData {
        int fAtlasId;
        SDL_Rect fCoords;
        SDL_Point fOffset;
    };

    std::vector<eCsvData> mSprites;
    std::vector<std::shared_ptr<eTexture>> mAtlases;
};

#endif // ESPRITELOADER_H
