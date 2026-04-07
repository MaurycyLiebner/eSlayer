#include "eblockaction.h"

#include "eserverunit.h"

#include <eSlayerHelpers/echardata.h>

std::shared_ptr<eBlockAction>
eBlockAction::sCreate(
    eServerUnit& unit, eServerArea& area) {
    const auto& data = unit.data();
    const int anim = data.animId("block");
    if(anim != -1) {
        const float fbr = unit.fasterBlockRate();
        const float efbr = std::floor(fbr*120.f/(fbr + 120.f));
        const int blockBase = data.animFrames(anim);
        const float animSpeed = 256.f;
        const int frames = int(std::ceil(256.f*blockBase/std::floor(animSpeed*(50.f + efbr)/100.f))) - 1;
        const auto a = std::make_shared<eBlockAction>(unit, area);
        a->setup(anim, frames, true, nullptr);
        return a;
    } else {
        return nullptr;
    }
}
