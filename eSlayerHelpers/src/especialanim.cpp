#include "eSlayerHelpers/especialanim.h"

#include "eSlayerHelpers/emissilesinfo.h"

bool eSpecialAnim::isSpecial(const uint8_t anim) {
    const bool fleshExpl = anim == sFleshExplAnim ||
                           anim == sFleshExplBody;
    const bool iceExpl = anim == sIceExplAnim ||
                         anim == sIceExplBody;
    return fleshExpl || iceExpl;
}

int eSpecialAnim::nFrames(const uint8_t anim) {
    const bool r = isSpecial(anim);
    if(!r) return 0;
    int mid;
    const bool fleshExpl = anim == sFleshExplAnim ||
                           anim == sFleshExplBody;
    if(fleshExpl) {
        mid = eMissilesInfo::sFleshId;
    } else { // if iceExpl
        mid = eMissilesInfo::sIceId;
    }
    const auto& info = eMissilesInfo::sMissiles.get(mid);
    const bool body = anim == sFleshExplBody ||
                      anim == sIceExplBody;
    int aid;
    if(body) {
        aid = info.stayAnimId();
    } else {
        aid = info.baseAnimId();
    }
    const auto& ainfo = info.anim(aid);
    return ainfo.fNFrames;
}
