#ifndef ECHARMODEL_H
#define ECHARMODEL_H

#include "etexturecollection.h"

#include <vector>
#include <memory>
#include <unordered_map>

using eCharTextureDirs = std::vector<std::shared_ptr<eTextureCollection>>;
using eCharTextureParts = std::vector<eCharTextureDirs>;
using eCharTextureGroups = std::vector<eCharTextureParts>;

struct eCharTextureAnim {
    int fFrames;
    std::string fClamp;
    eCharTextureGroups fGroups;
};

using eTextureSptr = std::shared_ptr<eTexture>;

class eCharTextures;

struct eTextureKey {
    int fAnim;
    int fFrame;
    int fDir;

    bool operator==(const eTextureKey& o) const {
        return fAnim == o.fAnim && fFrame == o.fFrame && fDir == o.fDir;
    }
};

struct eTextureKeyHash {
    size_t operator()(const eTextureKey& k) const {
        return (k.fAnim * 73856093) ^ (k.fFrame * 19349663) ^ (k.fDir * 83492791);
    }
};

class eCharModel {
    friend class eCharTextures;
public:
    eCharModel(const eCharTextures& data);

    const eTextureSptr& get(const int anim,
                            const int group,
                            const int part,
                            const int dir,
                            const int frame) const;

    const eCharTextures& data() const { return mData; }

    int nAnims() const { return mNAnims; }
    int nGroups() const { return mNGroups; }
    int nParts(const int group) const;
    int nDirs() const { return mNDirs; }
    int nFrames(const int anim) const;

    std::shared_ptr<eTexture> requestTexture(
        SDL_Renderer* const r,
        const eTextureKey& key);
    SDL_Rect requestBoundingRect(const eTextureKey& key);
private:
    SDL_Rect boundingRect(const eTextureKey& key) const;
    const eCharTextures& mData;

    int mNAnims = 0;
    int mNGroups = 0;
    std::vector<int> mNParts;
    int mNDirs = 0;

    std::vector<eCharTextureAnim> mAnims;
    std::unordered_map<eTextureKey, SDL_Rect, eTextureKeyHash> mRectCache;
    std::unordered_map<eTextureKey, std::shared_ptr<eTexture>, eTextureKeyHash> mTexCache;
};

#endif // ECHARMODEL_H
