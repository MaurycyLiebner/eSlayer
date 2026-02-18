#ifndef ESPRITELOADER_H
#define ESPRITELOADER_H

#include "etexture.h"

#include <SDL3/SDL_rect.h>

#include <string>
#include <vector>

class eTextureCollection;

class eSpriteLoader {
public:
    eSpriteLoader(const std::string& dir,
                  const std::string& path,
                  SDL_Renderer* const r,
                  const SDL_Color &colorKey = {0, 0, 0, 0});

    std::shared_ptr<eTexture> load(const int i);
    std::shared_ptr<eTexture> load(const int i,
                                   eTextureCollection& coll);
private:
    void initialize();

    const std::string mDir;
    const std::string mPath;
    SDL_Renderer* const mRenderer;
    const SDL_Color mColorKey = {0, 0, 0, 0};
    bool mInitialized = false;
    std::shared_ptr<eTexture> mAtlas;
    std::vector<SDL_Rect> mSpriteCoords;
    std::vector<SDL_Point> mSpriteOffsets;
};

#endif // ESPRITELOADER_H
