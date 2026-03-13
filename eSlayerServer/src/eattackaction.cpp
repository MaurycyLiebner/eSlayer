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
    const double iasItem = 0.; // all items speed modifiers
    const double eias = std::floor(iasItem*120./(iasItem + 120.));
    const double sias = 0.; // skill increased attack speed
    const double wsm = 0.; // weapon speed modifier
    const int animLen = data.animFrames(anim);
    const double animSpeed = 256.;
    const double animRate = 100.;
    const int frames = int(std::ceil(256.*animLen/std::floor(animSpeed*(animRate + sias + eias - wsm)/100.))) - 1;
    const auto result = std::make_shared<eAttackAction>(
        unit, area);
    result->setup(anim, frames, a);
    return result;
}
