#ifndef ETILETEXTURES_H
#define ETILETEXTURES_H

#include "../textures/etexturecollection.h"

#include <string>

class eTileTextures  {
public:
    eTileTextures();

    const std::shared_ptr<eTexture>& getTexture(const int id) const;
    int size() const;

    void load(const eResolution& res,
              SDL_Renderer * const r);
    void loadFixedSize(const int w, const int h,
                       const eResolution& res,
                       SDL_Renderer * const r);

    void initialize(const std::string& dirName,
                    const std::string& name);
private:
    bool mLoaded = false;
    std::string mDirName;
    std::string mName;
    std::shared_ptr<eTextureCollection> mColl;
};

#endif // ETILETEXTURES_H
