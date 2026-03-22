#include "eattackaction.h"

#include "eserverunit.h"

#include <eSlayerHelpers/echardata.h>
#include <eSlayerHelpers/eskills.h>

std::shared_ptr<eAttackAction>
eAttackAction::sCreate(
    eServerUnit& unit, eServerArea& area,
    const std::vector<int>& anims, const eAction& a) {
    const auto& data = unit.data();
    if(anims.empty()) return nullptr;
    const int anim = anims[eRand::rand() % anims.size()];
    const float iasItem = 0.f; // all items speed modifiers
    const float eias = std::floor(iasItem*120.f/(iasItem + 120.f));
    const float sias = 0.f; // skill increased attack speed
    const float wsm = 0.f; // weapon speed modifier
    const int animLen = data.animFrames(anim);
    const float animSpeed = 256.f;
    const float animRate = 100.f;
    const int frames = int(std::ceil(256.f*animLen/std::floor(animSpeed*(animRate + sias + eias - wsm)/100.f))) - 1;
    const auto result = std::make_shared<eAttackAction>(unit, area);
    result->setup(anim, frames, a);
    return result;
}
