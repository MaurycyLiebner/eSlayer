#ifndef ESPECIALANIM_H
#define ESPECIALANIM_H

#include "eslayerhelpersexport.h"

#include <cstdint>

const uint8_t sFleshExplAnim = 255;
const uint8_t sFleshExplBody = 254;
const uint8_t sIceExplAnim = 253;
const uint8_t sIceExplBody = 252;

namespace eSpecialAnim {
    bool ESLAYERHELPERS_API isSpecial(const uint8_t anim);
    int ESLAYERHELPERS_API nFrames(const uint8_t anim);
}

#endif // ESPECIALANIM_H
