#include "echarmodel.h"

eCharModel::eCharModel(const eCharTextures& data)
    : mData(data) {}

const eTextureSptr& eCharModel::get(const int anim, const int group, const int part, const int dir,
                                    const int frame) const {
    return mAnims[anim].fGroups[group][part][dir]->getTexture(frame);
}

int eCharModel::nParts(const int group) const {
    return mNParts[group];
}

int eCharModel::nFrames(const int anim) const {
    return mAnims[anim].fFrames;
}

const eOffset& eCharModel::animOffset(const int anim) const {
    return mAnims[anim].fOffset;
}
