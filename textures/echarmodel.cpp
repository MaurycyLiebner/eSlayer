#include "echarmodel.h"

const eTextureSptr& eCharModel::get(
    const int anim,
    const int group, const int part,
    const int dir, const int frame) const {
    return mAnims[anim].fGroups[group][part][dir]->getTexture(frame);
}
