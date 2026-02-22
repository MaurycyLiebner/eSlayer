#ifndef ECHARMODEL_H
#define ECHARMODEL_H

#include "etexturecollection.h"

#include <vector>
#include <memory>

using eCharTextureDirs = std::vector<std::shared_ptr<eTextureCollection>>;
using eCharTextureParts = std::vector<eCharTextureDirs>;
using eCharTextureGroups = std::vector<eCharTextureParts>;

struct eOffset {
    int fX;
    int fY;
};

struct eCharTextureAnim {
    int fFrames;
    eOffset fOffset;
    eCharTextureGroups fGroups;
};

using eTextureSptr = std::shared_ptr<eTexture>;

class eCharModel {
    friend class eCharTextures;
public:
    const eTextureSptr& get(const int anim,
                            const int group,
                            const int part,
                            const int dir,
                            const int frame) const;

    int nAnims() const { return mNAnims; }
    int nGroups() const { return mNGroups; }
    int nParts(const int group) const;
    int nDirs() const { return mNDirs; }
    int nFrames(const int anim) const;

    const eOffset& animOffset(const int anim) const;
private:
    int mNAnims = 0;
    int mNGroups = 0;
    std::vector<int> mNParts;
    int mNDirs = 0;
    std::vector<eCharTextureAnim> mAnims;
};

#endif // ECHARMODEL_H
