#include "eattackaction.h"

#include "eserverunit.h"

#include <eSlayerHelpers/echardata.h>
#include <eSlayerHelpers/eskills.h>

std::shared_ptr<eAttackAction> eAttackAction::sCreate(
    eServerUnit& unit, eServerArea& area,
    const std::vector<int>& anims,
    const eAttackType type, const eAction& a,
    const eSkillChoice schoice,
    const eWeaponChoice wchoice) {
    const auto& data = unit.data();
    if(anims.empty()) return nullptr;
    const int anim = anims[eRand::rand() % anims.size()];
    const int animLen = data.animFrames(anim);
    int frames;
    if(type == eAttackType::attack) {
        const float iasItem = 100.f*unit.itemsAttackSpeed(wchoice); // all items speed modifiers
        const float eias = std::floor(iasItem*120.f/(iasItem + 120.f));
        const float sias = 100.f*unit.skillsAttackSpeed(schoice); // skill increased attack speed
        const float wsm = unit.weaponSpeedModifier(wchoice); // weapon speed modifier
        const float animSpeed = 256.f;
        const float animRate = 100.f;
        frames = int(std::ceil(256.f*animLen/std::floor(animSpeed*(animRate + sias + eias - wsm)/100.f))) - 1;
    } else if(type == eAttackType::cast) {
        const float fcrItem = 100.f*unit.itemsCastRate(); // all items speed modifiers
        const float efcr = std::floor(fcrItem*120.f/(fcrItem + 120.f));
        const float animSpeed = 256.f;
        const float animRate = 100.f;
        frames = int(std::ceil(256.f*animLen/std::floor(animSpeed*(animRate + efcr)/100.f))) - 1;
    } else {
        frames = animLen;
    }
    const auto result = std::make_shared<eAttackAction>(unit, area);
    result->setup(anim, frames, a);
    return result;
}
