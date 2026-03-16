#include "eattackaction.h"

#include "eserverunit.h"

#include <eSlayerHelpers/echardata.h>

std::shared_ptr<eAttackAction>
eAttackAction::sCreate(
    eServerUnit& unit, eServerArea& area,
    const eAction& a) {
    const auto& data = unit.data();
    const int a1Id = data.animId("attack1");
    const int a2Id = data.animId("attack2");
    int anim;
    if(a2Id != -1 && eRand::rand() % 2) {
        anim = a2Id;
    } else if(a1Id != -1) {
        anim = a1Id;
    } else {
        return nullptr;
    }
    const float iasItem = 0.f; // all items speed modifiers
    const float eias = std::floor(iasItem*120.f/(iasItem + 120.f));
    const float sias = 0.f; // skill increased attack speed
    const float wsm = 0.f; // weapon speed modifier
    const int animLen = data.animFrames(anim);
    const float animSpeed = 256.f;
    const float animRate = 100.f;
    const int frames = int(std::ceil(256.f*animLen/std::floor(animSpeed*(animRate + sias + eias - wsm)/100.f))) - 1;
    const auto result = std::make_shared<eAttackAction>(
        unit, area);
    result->setup(anim, frames, a);
    return result;
}
