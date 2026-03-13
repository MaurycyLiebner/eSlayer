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
    const auto result = std::make_shared<eAttackAction>(
        unit, area);
    result->setup(anim, a);
    return result;
}
