#include "eblockaction.h"

#include "eserverunit.h"

#include <eSlayerHelpers/echardata.h>

std::shared_ptr<eBlockAction>
eBlockAction::sCreate(
    eServerUnit& unit, eServerArea& area) {
    const auto& data = unit.data();
    const int anim = data.animId("block");
    if(anim != -1) {
        const double fbr = unit.fasterBlockRate();
        const double efbr = std::floor(fbr*120./(fbr + 120.));
        const int blockBase = data.animFrames(anim);
        const double animSpeed = 256.;
        const int frames = int(std::ceil(256.*blockBase/std::floor(animSpeed*(50. + efbr)/100.))) - 1;
        const auto a = std::make_shared<eBlockAction>(unit, area);
        a->setup(anim, frames, nullptr);
        return a;
    } else {
        return nullptr;
    }
}
